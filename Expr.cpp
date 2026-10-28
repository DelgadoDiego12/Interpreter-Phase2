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

ValueDescriptor *BinaryExprNode::evaluate(const SymbolTable &symbolTable) const {

    ValueDescriptor *leftValue = leftOperand->evaluate(symbolTable);
    ValueDescriptor *rightValue = rightOperand->evaluate(symbolTable);

    const bool leftIsDouble = leftValue->type() == ValueDescriptor::Type::doubleValue;

    const bool rightIsDouble = rightValue->type() == ValueDescriptor::Type::doubleValue;

    if (leftIsDouble || rightIsDouble) {
        double left;
        double right;

        if (leftIsDouble)
            left = dynamic_cast<DoubleValueDescriptor *>(leftValue)->value();
        else
            left = dynamic_cast<IntegerValueDescriptor *>(leftValue)->value();

        if (rightIsDouble)
            right = dynamic_cast<DoubleValueDescriptor *>(rightValue)->value();
        else
            right = dynamic_cast<IntegerValueDescriptor *>(rightValue)->value();

        ValueDescriptor *result = nullptr;

        if (token().isAdditionOperator())
            result = new DoubleValueDescriptor{left + right};
        else if (token().isSubtractionOperator())
            result = new DoubleValueDescriptor{left - right};
        else if (token().isMultiplicationOperator())
            result = new DoubleValueDescriptor{left * right};
        else if (token().isDivisionOperator()){
            if (right == 0.0)
                throw std::runtime_error("Division by zero");
            result = new DoubleValueDescriptor{left / right};
        }
        else if (token().isEqualToOperator())
            result = new BooleanValueDescriptor{left == right};
        else if (token().isNotEqualToOperator())
            result = new BooleanValueDescriptor{left != right};
        else if (token().isLessThanOperator())
            result = new BooleanValueDescriptor{left < right};
        else if (token().isGreaterThanOperator())
            result = new BooleanValueDescriptor{left > right};
        else if (token().isLessOrEqualToOperator())
            result = new BooleanValueDescriptor{left <= right};
        else if (token().isGreaterOrEqualToOperator())
            result = new BooleanValueDescriptor{left >= right};

        delete leftValue;
        delete rightValue;

        if (result != nullptr)
            return result;

        throw std::runtime_error("Unsupported double operator");
    }

    const auto left = dynamic_cast<IntegerValueDescriptor *>(leftValue)->value();
    const auto right = dynamic_cast<IntegerValueDescriptor *>(rightValue)->value();

    ValueDescriptor *result = nullptr;

    if (token().isAdditionOperator())
        result = new IntegerValueDescriptor{left + right};
    else if (token().isSubtractionOperator())
        result = new IntegerValueDescriptor{left - right};
    else if (token().isMultiplicationOperator())
        result = new IntegerValueDescriptor{left * right};
    else if (token().isDivisionOperator()) {
        if (right == 0)
            throw std::runtime_error("Division by zero");
        result = new DoubleValueDescriptor{static_cast<double>(left) / static_cast<double>(right)};
    }
    else if (token().isModuloOperator()) {
        if (right == 0)
            throw std::runtime_error("Division by zero");
        result = new IntegerValueDescriptor{left % right};
    }
    else if (token().isEqualToOperator())
        result = new BooleanValueDescriptor{left == right};
    else if (token().isNotEqualToOperator())
        result = new BooleanValueDescriptor{left != right};
    else if (token().isLessThanOperator())
        result = new BooleanValueDescriptor{left < right};
    else if (token().isGreaterThanOperator())
        result = new BooleanValueDescriptor{left > right};
    else if (token().isLessOrEqualToOperator())
        result = new BooleanValueDescriptor{left <= right};
    else if (token().isGreaterOrEqualToOperator())
        result = new BooleanValueDescriptor{left >= right};

    delete leftValue;
    delete rightValue;

    if (result != nullptr)
        return result;

    throw std::runtime_error("Unsupported integer operator");
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

ValueDescriptor *UnaryExprNode::evaluate(const SymbolTable &symbolTable) const {
    ValueDescriptor *value = expression->evaluate(symbolTable);

    if (value->type() == ValueDescriptor::Type::integer) {
        const auto integerValue = dynamic_cast<IntegerValueDescriptor *>(value)->value();

        if (token().isAdditionOperator())
            return value;

        if (token().isSubtractionOperator()) {
            delete value;
            return new IntegerValueDescriptor{-integerValue};
        }
    }

    if (value->type() == ValueDescriptor::Type::doubleValue) {
        const auto doubleValue = dynamic_cast<DoubleValueDescriptor *>(value)->value();

        if (token().isAdditionOperator())
            return value;

        if (token().isSubtractionOperator()) {
            delete value;
            return new DoubleValueDescriptor{-doubleValue};
        }
    }
    delete value;
    throw std::runtime_error("Unsupported prefix operator");
}

void UnaryExprNode::print() const {
    std::cout << token().symbol();
    expression->print();
}

IntegerLiteral::IntegerLiteral(Token token) : ExprNode{std::move(token)} {}

void IntegerLiteral::print() const {
    token().print(std::cout);
}

ValueDescriptor * IntegerLiteral::evaluate(const SymbolTable &) const {
    return new IntegerValueDescriptor{token().integerValue()};
}

DoubleLiteral::DoubleLiteral(Token token) : ExprNode{std::move(token)} {}

void DoubleLiteral::print() const {
    token().print(std::cout);
}

ValueDescriptor *DoubleLiteral::evaluate(const SymbolTable &) const {
    return new DoubleValueDescriptor{token().doubleValue()};
}

BooleanLiteral::BooleanLiteral(Token token) : ExprNode{std::move(token)} {}

void BooleanLiteral::print() const {
    token().print(std::cout);
}

ValueDescriptor *BooleanLiteral::evaluate(const SymbolTable &) const {
    return new BooleanValueDescriptor(token().isTrueKeyword()); //returns keyword not token
}

StringLiteral::StringLiteral(Token token) : ExprNode{std::move(token)} {}

void StringLiteral::print() const {
    token().print(std::cout);
}

ValueDescriptor *StringLiteral::evaluate(const SymbolTable &) const {
    return new StringValueDescriptor{token().stringValue()};
}

Variable::Variable(Token token) : ExprNode{std::move(token)} {}

void Variable::print() const {
    token().print(std::cout);
}

ValueDescriptor *Variable::evaluate(const SymbolTable &symbolTable) const {
    return symbolTable.getValueFor(token().identifier())->clone();
}


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
    auto getIntegerValue = [&symbolTable](ExprNode *expression)->int {
        ValueDescriptor *value = expression->evaluate(symbolTable);

        if (value->type() != ValueDescriptor::Type::integer) {
            delete value;
            throw std::runtime_error("range arguments must be integers");
        }
        const int integerValue = dynamic_cast<IntegerValueDescriptor *>(value)->value();
        delete value;
        return integerValue;
    };
    const int start = startExpression != nullptr ? getIntegerValue(startExpression) : 0;
    const int stop = getIntegerValue(stopExpression);
    const int step = stepExpression != nullptr ? getIntegerValue(stepExpression) : 1;

    return EvaluatedRange{start, stop, step};
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