#include <cstdlib>
#include <iostream>
#include <string>

#include "Parser.hpp"

void Parser::die(const std::string &where,
                 const std::string &message,
                 const Token &token) const {
    std::cerr << where << ": " << message << " at line " << token.lineNumber()
              << ", column " << token.columnNumber() << ". Got: ";
    token.print(std::cerr);
    std::cerr << "\n\nTokens identified up to this point:\n";
    tokenizer.printProcessedTokens(std::cerr);
    std::exit(EXIT_FAILURE);
}

Statements *Parser::program() {
    // <program> -> <statements> EOF
    Statements *parsedStatements = statements();
    Token eof = tokenizer.getToken();
    if (!eof.isEof()) {
        delete parsedStatements;
        die("Parser::program", "expected EOF", eof);
    }
    return parsedStatements;
}

Statements *Parser::statements() {
    // new version <statements> -> <statement> { <statement> }
    auto *parsedStatements = new Statements();
    parsedStatements->addStatement(statement());

    Token next = tokenizer.getToken();

    while (next.isIdentifier() || next.isPrintKeyword() || next.isForKeyword()) {
        tokenizer.ungetToken();
        parsedStatements->addStatement(statement());
        next = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return parsedStatements;
}


Statements *Parser::suite(){
    //<suite> -> NEWLINE INDENT <statements> DEDENT
    Token newline = tokenizer.getToken();
    if (!newline.isNewline()) {
        die("Parser::suite", "expected NEWLINE after statement", newline);
    }
    Token indent = tokenizer.getToken();
    if (!indent.isIndent()) {
        die("Parser::suite", "expected indent", indent);
    }
    Statements *loopBody = statements();

    Token dedent = tokenizer.getToken();
    if (!dedent.isDedent()){
        delete loopBody;
        die("Parser::suite", "expected dedent", dedent);
    }
    return loopBody;
}

Statement *Parser::statement() {
    // <statement> -> <simple-statement> NEWLINE
    //             | <compound-statement>

   Token token = tokenizer.getToken();

    if (token.isIdentifier() || token.isPrintKeyword()) {
        tokenizer.ungetToken();

        Statement *simple = simpleStatement();

        Token newline = tokenizer.getToken();
        if (!newline.isNewline()) {
            delete simple;
            die("Parser::statement", "expected NEWLINE after simple statement", newline);
        }
        return simple;
    }
    if (token.isForKeyword()) {
        tokenizer.ungetToken();
        return compoundStatement();
    }
    die("Parser::statement","expected a statement",token);
}

Statement *Parser::simpleStatement() {
    //<simple-statement> -> <assignment-statement>
    //                     | <print-statement>
    Token token = tokenizer.getToken();
    if (token.isIdentifier()) {
        tokenizer.ungetToken();
        return assignmentStatement();
    }
    if (token.isPrintKeyword()) {
        tokenizer.ungetToken();
        return printStatement();
    }
    die("Parser::simpleStatement", "expected assignment or print statement", token);

}

Statement *Parser::compoundStatement() {
    //<compound-statement> -> <for-statement>
    return forStatement();
}

AssignmentStatement *Parser::assignmentStatement() {
    // <assignment-statement> -> <id> = <rel-expr>
    // The caller consumes the context-dependent terminator: NEWLINE in a
    // statement list or ';' in a future for-loop header.
    Token variable = tokenizer.getToken();
    if (!variable.isIdentifier())
        die("Parser::assignmentStatement", "expected an identifier", variable);

    Token assignmentOperator = tokenizer.getToken();
    if (!assignmentOperator.isAssignmentOperator())
        die("Parser::assignmentStatement", "expected '='", assignmentOperator);

    return new AssignmentStatement(variable.identifier(), relExpr());
}

PrintStatement *Parser::printStatement() {
    // <print-statement> -> print <rel-expr>
    Token printToken = tokenizer.getToken();
    if (!printToken.isPrintKeyword()) {
        die ("Parser::printStatement", "expected 'print'", printToken);
    }
    return new PrintStatement(relExpr());
}

RangeExpression *Parser::rangeExpression() {
    // <range> -> "range" "(" <range-arguments> ")"
    Token rangeToken = tokenizer.getToken();
    if (!rangeToken.isRangeKeyword()) {
        die("Parser::rangeExpression", "expected 'range'", rangeToken);
    }
    Token openParenToken = tokenizer.getToken();
    if (!openParenToken.isOpenParen()){
        die ("Parser::forStatement", "expected ' ( '", openParenToken);
    }
    ExprNode *firstArg = relExpr();
    ExprNode *secondArg = nullptr;
    ExprNode *thirdArg = nullptr;

    Token next = tokenizer.getToken(); //to check what version of range is being used
    if (next.isComma()) {
        secondArg = relExpr();
        next = tokenizer.getToken();
        if (next.isComma()) {
            thirdArg = relExpr();
            next = tokenizer.getToken();
        }
    }
    if (!next.isCloseParen()) {
        delete firstArg;
        delete secondArg;
        delete thirdArg;
        die("Parser::rangeStatement", "expected ')'", next);
    }
    if (thirdArg != nullptr) {
        return new RangeExpression(firstArg, secondArg, thirdArg);
    }
    if (secondArg != nullptr) {
        return new RangeExpression(firstArg, secondArg);
    }
    return new RangeExpression(firstArg);
}

ForStatement *Parser::forStatement() {
    // <for-statement> -> for <id> in <range> : <suite>
    Token forToken = tokenizer.getToken();
    if (!forToken.isForKeyword()) {
        die ("Parser::forStatement", "expected 'for'", forToken);
    }
    Token variable = tokenizer.getToken();
    if (!variable.isIdentifier())
        die("Parser::forStatement", "expected an identifier", variable);

    Token inToken = tokenizer.getToken();
    if (!inToken.isInKeyword()){
        die("Parser::forStatement", "expected 'in'", inToken);
    }
    RangeExpression *range = rangeExpression();

    Token colonToken = tokenizer.getToken();
    if (!colonToken.isColon()) {
        delete range;
        die("Parser::forStatement", "expected ':'", colonToken);
    }

    Statements *loopBody = suite();

    return new ForStatement(variable.identifier(),range, loopBody);

}

ExprNode *Parser::relExpr() {
    // <rel-expr> -> <rel-term> [ <equality-op> <rel-term> ]
    // The optional equality operation is left for students to implement.
    ExprNode *left = relTerm();
    Token token = tokenizer.getToken();

    if (token.isEqualToOperator() || token.isNotEqualToOperator()) {
        ExprNode *right = relTerm();
        left = new BinaryExprNode(token, left, right);
    }  else {
        tokenizer.ungetToken();

    }
    return left;
}

ExprNode *Parser::relTerm() {
    // <rel-term> -> <rel-primary> [ <ordering-op> <rel-primary> ]
    // The optional ordering operation is left for students to implement.
    ExprNode *left = relPrimary();
    Token token = tokenizer.getToken();

    if (token.isGreaterThanOperator() ||
        token.isLessThanOperator() ||
        token.isGreaterOrEqualToOperator()||
        token.isLessOrEqualToOperator()) {

        ExprNode *right = relPrimary();
        left = new BinaryExprNode(token, left, right);
        } else {
            tokenizer.ungetToken();
        }
    return left;
}

ExprNode *Parser::relPrimary() {
    // <rel-primary> -> <arith-expr>

    return arithExpr();
}

ExprNode *Parser::arithExpr() {
    // <arith-expr> -> <arith-term> { <add-op> <arith-term> }
    ExprNode *left = arithTerm();
    Token token = tokenizer.getToken();

    while (token.isAdditionOperator() || token.isSubtractionOperator()) {
        ExprNode *right = arithTerm();
        left = new BinaryExprNode(token, left, right);
        token = tokenizer.getToken();
    }

    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::arithTerm() {
    // <arith-term> -> <arith-primary> { <mult-op> <arith-primary> }
    ExprNode *left = arithPrimary();
    Token token = tokenizer.getToken();

    while (token.isMultiplicationOperator() ||
           token.isDivisionOperator() ||
           token.isModuloOperator()) {
        ExprNode *right = arithPrimary();
        left = new BinaryExprNode(token, left, right);
        token = tokenizer.getToken();
    }

    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::arithPrimary() {
    // <arith-primary> -> [ <sign> ] <arith-atom>
    Token token = tokenizer.getToken();
    if (token.isAdditionOperator() || token.isSubtractionOperator())
        return new UnaryExprNode(token, arithAtom());

    tokenizer.ungetToken();
    return arithAtom();
}

ExprNode *Parser::arithAtom() {
    // <arith-atom> -> <id> | <integer> | '(' <rel-expr> ')'
    Token token = tokenizer.getToken();

    if (token.isInteger())
        return new IntegerLiteral(token);
    if (token.isIdentifier())
        return new Variable(token);
    if (token.isOpenParen()) {
        ExprNode *expression = relExpr();
        Token closeParen = tokenizer.getToken();
        if (!closeParen.isCloseParen())
            die("Parser::arithAtom", "expected ')'", closeParen);
        return expression;
    }

    die("Parser::arithAtom", "expected an identifier, integer, or '('", token);
}
