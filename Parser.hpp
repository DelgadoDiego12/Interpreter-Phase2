#ifndef EXPRINTER_PARSER_HPP
#define EXPRINTER_PARSER_HPP

#include <string>

#include "Expr.hpp"
#include "Statements.hpp"
#include "Token.hpp"
#include "Tokenizer.hpp"

class Parser {
public:
    explicit Parser(Tokenizer &tokenizer) : tokenizer{tokenizer} {}

    Statements *program();
    Statements *statements();
    Statements *suite();
    Statement *statement();
    Statement *simpleStatement();
    Statement *compoundStatement();
    AssignmentStatement *assignmentStatement();
    PrintStatement *printStatement();
    AppendStatement *appendStatement(); //
    ForStatement *forStatement();
    IfStatement *ifStatement(); //
    WhileStatement *whileStatement(); //

    ExprNode *relExpr();
    ExprNode *relTerm();
    ExprNode *relPrimary();
    ExprNode *arithExpr();
    ExprNode *arithTerm();
    ExprNode *arithPrimary();
    ExprNode *postfixExpr();
    ExprNode *atom();
    ExprNode *literal();
    ExprNode *inputExpr();
    ExprNode *oneArgCall();
    ExprNode *oneArgExpr(Token token, ExprNode *argument);
    ExprNode *listLiteral();

private:
    Tokenizer &tokenizer;
    RangeExpression *rangeExpression();
    [[noreturn]] void die(const std::string &where,
                          const std::string &message,
                          const Token &token) const;
};

#endif // EXPRINTER_PARSER_HPP;
