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

    while (next.isIdentifier() || next.isPrintKeyword() || next.isForKeyword()  
                               || next.isIfKeyword() || next.isElifKeyword() 
                               || next.isElseKeyword() || next.isWhileKeyword()) {
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
    if (token.isForKeyword() || token.isIfKeyword() || token.isElifKeyword() 
                             || token.isElseKeyword() || token.isWhileKeyword()) {
        tokenizer.ungetToken();
        return compoundStatement();
    }
    die("Parser::statement","expected a statement",token);
}

Statement *Parser::simpleStatement() {
    //<simple-statement> -> <assignment-statement>
    //                      |<print-statement>
    //                      |<append-statement>
    Token token = tokenizer.getToken();
    if (token.isIdentifier()) {
        tokenizer.ungetToken();
        return assignmentStatement();
    }
    if (token.isPrintKeyword()) {
        tokenizer.ungetToken();
        return printStatement();
    }
    if (token.isAppendKeyword()) {
        tokenizer.ungetToken();
        return appendStatement();
    }
    die("Parser::simpleStatement", "expected assignment or print statement", token);

}

Statement *Parser::compoundStatement() {
    //<compound-statement> -> <for-statement>
    //                       |<if-statement>
    //                       |<while-statement>
    Token token = tokenizer.getToken();
    if (token.isForKeyword()) {
        tokenizer.ungetToken();
        return forStatement();
    }
    if (token.isIfKeyword() || token.isElseKeyword() || token.isElifKeyword()) {
        tokenizer.ungetToken();
        return ifStatement();
    }
    if (token.isWhileKeyword()) {
        tokenizer.ungetToken();
        return whileStatement();
    }
    die("Parser::compuntStatement", "expected 'for', 'if', or 'while'", token);
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

AppendStatement *Parser::appendStatement() {
    //<append-statement> -> append ( <rel-expr> , <rel-expr> )
    Token appendToken = tokenizer.getToken();
    if (!appendToken.isAppendKeyword()) {
        die ("Parser::appendStatement", "expected 'append'", appendToken);
    }
    Token openParenToken = tokenizer.getToken();
    if (!openParenToken.isOpenParen()) {
        die("Parser::appendStatement", "expected '('", openParenToken);
    }
    ExprNode *expression1 = relExpr();
    Token commaToken = tokenizer.getToken();
    if (!commaToken.isComma()) {
        die("Parser::appendStatement", "expected ','", commaToken);
    }
    ExprNode *expression2 = relExpr();
    Token closeParen = tokenizer.getToken();
    if (!closeParen.isCloseParen()){
        die("Parser::appendStatement", "expected ')'", closeParen);
    }
    return new AppendStatement(expression1, expression2);
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

IfStatement *Parser::ifStatement() {
    //<if-statement> -> if <rel-expr> : <suite>
    Token conditionalToken = tokenizer.getToken();
    if (!conditionalToken.isIfKeyword() && !conditionalToken.isElifKeyword() && !conditionalToken.isElseKeyword()) {
        die("Parser::ifStatement", "expected 'if/elif/else'", conditionalToken);
    }
    if (conditionalToken.isElseKeyword()) {
        Token colonToken = tokenizer.getToken();
        if (!colonToken.isColon()) {
            die("Parser::ifStatement", "expected ':'", colonToken);
        }
        Statements *body = suite();
        return new IfStatement(body);
    }
    ExprNode *condition = relExpr();
    Token colonToken = tokenizer.getToken();
    if (!colonToken.isColon()) {
        die("Parser::ifStatement", "expected ':'", colonToken);
    }
    Statements *body = suite();
    return new IfStatement(condition, body);
}

WhileStatement *Parser::whileStatement() {
    //<while-statement> -> while <rel-expr> : <suite>
    Token whileToken = tokenizer.getToken();
    if (!whileToken.isWhileKeyword()) {
        die ("Parser::whileStatement", "expected 'while'", whileToken);
    }
    ExprNode *condition = relExpr();
    Token colonToken = tokenizer.getToken();
    if (!colonToken.isColon()) {
        die ("Parser::whileStatement", "expected 'colon'", whileToken);
    }
    Statements *body = suite();
    return new WhileStatement(condition, body);
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
           token.isFloorDivisionOperator() ||
           token.isModuloOperator()) {
        ExprNode *right = arithPrimary();
        left = new BinaryExprNode(token, left, right);
        token = tokenizer.getToken();
    }

    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::arithPrimary() {
    // <arith-primary> -> [ <sign> ] <postfix-expression>
    Token token = tokenizer.getToken();
    if (token.isAdditionOperator() || token.isSubtractionOperator())
        return new UnaryExprNode(token, atom());

    tokenizer.ungetToken();
    return postfixExpr();
}

ExprNode *Parser::postfixExpr(){ //unfinished
    //<postfix-expression> <atom> [ [ <rel-expr> ] ]
    ExprNode *atomToken = atom();
    Token openBrackToken = tokenizer.getToken();
    if (!openBrackToken.isOpenBracket()) {
        die("Parser::postfixExpr", "expected '['", openBrackToken);
    }
    ExprNode *expression = relExpr();
    Token closeBrackToken = tokenizer.getToken();
    if (!closeBrackToken.isCloseBracket()) {
        delete expression;
        die("Parser::postfixEcpr", "expected ']'", closeBrackToken);
    }
    return atom;
}

ExprNode *Parser::atom() { //missing list literal
    //<atom> -> <id> | <literal> | <list-literal> | <input-expression> | <one-argument-call> | (<rel-expr>)
    Token token = tokenizer.getToken();

    if (token.isInteger() || token.isDouble() || token.isTrueKeyword() || token.isFalseKeyword()
        || token.isString()) {

        tokenizer.ungetToken();
        return literal();
    }

    if (token.isIdentifier()) {
        return new Variable(token);
    }
    if (token.isOpenParen()) {
        ExprNode *expression = relExpr();
        Token closeParen = tokenizer.getToken();
        if (!closeParen.isCloseParen())
            die("Parser::atom", "expected ')'", closeParen);
        return expression;
    }
    if (token.isIntKeyword() || token.isFloatKeyword() || token.isSTRKeyword() || token.isLenKeyword() || token.isSplitKeyword() || token.isPopKeyword()) {
        tokenizer.ungetToken();
        return oneArgCall();
    }
    if (token.isInputKeyword()) {
        tokenizer.ungetToken();
        return inputExpr();
    }
    die("Parser::atom", "expected an identifier, literal, or '('", token);
}

ExprNode *Parser::literal() {
    //<literal> -> <integer> | <double> | <boolean> | <string>
    Token token = tokenizer.getToken();
    if (token.isInteger())
        return new IntegerLiteral(token);
    if (token.isDouble())
        return new DoubleLiteral(token);
    if (token.isTrueKeyword() || token.isFalseKeyword() )
        return new BooleanLiteral(token);
    if (token.isString())
        return new StringLiteral(token);
    die("Parser::literal", "expected a literal", token);
}


ExprNode *Parser::inputExpr(){
        //<input-expr> -> input ( )
        Token token = tokenizer.getToken();
        if (!token.isInputKeyword()) {
            die("Parser::inputExpr", "expected 'input'", token);
        }
        Token openParenToken = tokenizer.getToken();
        if (!openParenToken.isOpenParen()) {
            die("Parser::inputExpr", "expected '('", openParenToken);
        }
        Token closeParenToken = tokenizer.getToken();
        if (!closeParenToken.isCloseParen()) {
            die("Parser::inputExpr", "expected ')'", closeParenToken);
        }
        return new UnaryExprNode(token, postfixExpr());
}

ExprNode *Parser::oneArgCall() {

    //try to reuse the parenthesis code for all 
    Token nameToken = tokenizer.getToken();
    if (!(nameToken.isIntKeyword() || nameToken.isFloatKeyword() || nameToken.isSTRKeyword() || nameToken.isLenKeyword() || nameToken.isSplitKeyword() || nameToken.isPopKeyword())) {
        die("Parser::oneArgCall", "expected 'int', 'float', 'str', 'len', 'split', or 'pop'", nameToken);
    }
    
    Token openParen = tokenizer.getToken();

    if (!openParen.isOpenParen())
        die("Parser::atom", "expected '('", openParen);

    ExprNode *argument = relExpr();
    Token closeParen = tokenizer.getToken();

    if (!closeParen.isCloseParen()) {
        delete argument;
        die("Parser::oneArgCall", "expected ')'", closeParen);
    }
    return oneArgExpr(nameToken, argument);
    
}

ExprNode *Parser::oneArgExpr(Token token, ExprNode *argument) {

    if (token.isSTRKeyword()) {
        return new StringConversion{argument};
    }
    if (token.isIntKeyword()) {
        return new IntegerConversion{argument};
    }
    if (token.isFloatKeyword()) {
        return new FloatConversion{argument};
    }
    if (token.isLenKeyword()) {
        return new LengthConversion{argument};
    }
    if (token.isSplitKeyword()) {
        return new SplitConversion{argument};
    }
    if (token.isPopKeyword()) {
        return new PopConversion{argument};
    }
    die("Parser::oneArgExpr", "expected 'int', 'float', 'str', 'len', 'split', or 'pop'", token);
}

ExprNode *Parser::listLiteral() {

}