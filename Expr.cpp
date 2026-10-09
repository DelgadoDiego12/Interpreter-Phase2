#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <iosfwd>
#include "Expr.hpp"

ExprNode::ExprNode(Token token) : expressionToken{std::move(token)} {}

const Token &ExprNode::token() const {
    return expressionToken;
}

BinaryExprNode::BinaryExprNode(Token token, ExprNode *left, ExprNode *right)
    : ExprNode{std::move(token)},
      leftOperand{left},
      rightOperand{right} {}

BinaryExprNode::~BinaryExprNode() {
    delete leftOperand;
    delete rightOperand;
}

int BinaryExprNode::evaluate(const SymbolTable &symbolTable) const {
    const int leftValue = leftOperand->evaluate(symbolTable);
    const int rightValue = rightOperand->evaluate(symbolTable);

    if (token().isAdditionOperator())
        return leftValue + rightValue;
    if (token().isSubtractionOperator())
        return leftValue - rightValue;
    if (token().isMultiplicationOperator())
        return leftValue * rightValue;
    if (token().isDivisionOperator()) {
        if (rightValue == 0)
            throw std::runtime_error(
                "division by zero at line " + std::to_string(token().lineNumber()) +
                ", column " + std::to_string(token().columnNumber()));
        return leftValue / rightValue;
    }
    if (token().isModuloOperator()) {
        if (rightValue == 0)
            throw std::runtime_error(
                "modulo by zero at line " + std::to_string(token().lineNumber()) +
                ", column " + std::to_string(token().columnNumber()));
        return leftValue % rightValue;
    }
    if (token().isEqualToOperator())
        return leftValue == rightValue;
    if (token().isNotEqualToOperator())
        return leftValue != rightValue;
    if (token().isLessThanOperator())
        return leftValue < rightValue;
    if (token().isGreaterThanOperator())
        return leftValue > rightValue;
    if (token().isLessOrEqualToOperator())
        return leftValue <= rightValue;
    if (token().isGreaterOrEqualToOperator())
        return leftValue >= rightValue;

    throw std::logic_error("unsupported infix operator");
}

void BinaryExprNode::print() const {
    std::cout << '(';
    leftOperand->print();
    token().print(std::cout);
    rightOperand->print();
    std::cout << ')';
}

UnaryExprNode::UnaryExprNode(Token token, ExprNode *operand)
    : ExprNode{std::move(token)}, expression{operand} {}

UnaryExprNode::~UnaryExprNode() {
    delete expression;
}

int UnaryExprNode::evaluate(const SymbolTable &symbolTable) const {
    const int value = expression->evaluate(symbolTable);
    if (token().isAdditionOperator())
        return value;
    if (token().isSubtractionOperator())
        return -value;
    throw std::logic_error("unsupported prefix operator");
}

void UnaryExprNode::print() const {
    std::cout << token().symbol();
    expression->print();
}

IntegerLiteral::IntegerLiteral(Token token) : ExprNode{std::move(token)} {}

void IntegerLiteral::print() const {
    token().print(std::cout);
}

int IntegerLiteral::evaluate(const SymbolTable &) const {
    return token().integerValue();
}

Variable::Variable(Token token) : ExprNode{std::move(token)} {}

void Variable::print() const {
    token().print(std::cout);
}

int Variable::evaluate(const SymbolTable &symbolTable) const {
    return symbolTable.getValueFor(token().identifier());
}

///range stuff

EvaluatedRange::EvaluatedRange(int start, int stop, int step) : start_(start), stop_(stop), step_(step){
    if (step_ == 0) {
        throw std::runtime_error("Invalid step amount"); //change to runtime error
    }
}

int EvaluatedRange::start() const {
    return start_;
}

int EvaluatedRange::stop() const {
    return stop_;
}

int EvaluatedRange::step() const {
    return step_;
}

bool EvaluatedRange::hasIteration() const {
    if (step_ > 0) { //for positive step
        return start_ < stop_;
    }
    else { //else for negative step
        return start_ > stop_;
    }
}
//similar to has iteration but you check if the next value is valid instead of start_
bool EvaluatedRange::shouldContinue(int nextValue) const {
    if (step_ > 0) {
        return nextValue < stop_;
    }
    else {
        return nextValue > stop_;
    }
}

//range expression methods
//first form of range constructor
RangeExpression::RangeExpression(ExprNode* stop) : startExpression{nullptr}, stopExpression{stop}, stepExpression{nullptr} {}
//second form of range constructor
RangeExpression::RangeExpression(ExprNode* start, ExprNode* stop) : startExpression{start}, stopExpression{stop}, stepExpression {nullptr} {}
//third form of range constructor
RangeExpression::RangeExpression(ExprNode* start, ExprNode* stop, ExprNode* step) : startExpression{start}, stopExpression{stop}, stepExpression{step} {}

RangeExpression::~RangeExpression() {
    delete startExpression;
    delete stopExpression;
    delete stepExpression;
}


EvaluatedRange RangeExpression::evaluate(const SymbolTable &symbolTable) const {
    int start, stop, step;
    if (startExpression != nullptr) { //if not null evaluate
        start = startExpression->evaluate(symbolTable);
    }
    else { start = 0; } //otherwise set start to 0
    stop = stopExpression->evaluate(symbolTable);
    if (stepExpression != nullptr) { //if not null also evaluate
        step = stepExpression->evaluate(symbolTable);
    }
    else { step = 1; } //if null set to 1
    return EvaluatedRange(start, stop, step);
}

void RangeExpression::print() const {
    std::cout << "range( ";
    if (startExpression != nullptr) {
        startExpression->print();
        std::cout << " , ";
    }
    stopExpression->print();
    if (stepExpression != nullptr) {
        std::cout << " , ";
        stepExpression->print();
    }
    std::cout << " )";
}