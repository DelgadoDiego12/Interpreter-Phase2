#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include "Statements.hpp"

Statements::~Statements() {
    for (auto *statement : statements)
        delete statement;
}

void Statements::addStatement(Statement *statement) {
    statements.push_back(statement);
}

void Statements::print() const {
    for (const auto *statement : statements)
        statement->print();
}

void Statements::evaluate(SymbolTable &symbolTable) const {
    for (const auto *statement : statements)
        statement->evaluate(symbolTable);
}

AssignmentStatement::AssignmentStatement(
    std::string variableName,
    ExprNode *expression)
    : variableName{std::move(variableName)}, expression{expression} {}

AssignmentStatement::~AssignmentStatement() {
    delete expression;
}

void AssignmentStatement::evaluate(SymbolTable &symbolTable) const {
    symbolTable.setValueFor(variableName, expression->evaluate(symbolTable));
}

void AssignmentStatement::print() const {
    std::cout << variableName << " = ";
    expression->print();
    std::cout << '\n';
}

void AssignmentStatement::otherPrint() const {
    std::cout << variableName << " = ";
    expression->print();
}

PrintStatement::PrintStatement(ExprNode *expression) : expression{expression} {}

PrintStatement::~PrintStatement() {
    delete expression;
}

void PrintStatement::evaluate(SymbolTable &symbolTable) const {
    std::cout << expression->evaluate(symbolTable) << '\n';
}

void PrintStatement::print() const {
    std::cout << "print ";
    expression->print();
    std::cout << '\n';
}

ForStatement::ForStatement(std::string variableName, RangeExpression *rangeExpression, Statements *loopBody) : variableName{std::move(variableName)}, rangeExpression{rangeExpression}, loopBody{loopBody} {}


ForStatement::~ForStatement() {
    delete rangeExpression;
    delete loopBody;
}


void ForStatement::evaluate(SymbolTable &symbolTable) const {

    EvaluatedRange range = rangeExpression->evaluate(symbolTable);
    int iterator = range.start();
    while (range.shouldContinue(iterator)) {
        symbolTable.setValueFor(variableName, iterator);
        loopBody->evaluate(symbolTable);
        iterator += range.step();
    }
}

void ForStatement::print() const {
    std::cout << "for " << variableName << " in ";
    rangeExpression->print();
    loopBody->print();
}
