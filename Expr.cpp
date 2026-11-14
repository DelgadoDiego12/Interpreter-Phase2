#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <regex>
#include <cmath>
#include <charconv>
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

    const bool leftIsString = leftValue->type() == ValueDescriptor::Type::string;
    const bool rightIsString = rightValue->type() == ValueDescriptor::Type::string;

    if (leftIsString || rightIsString) {
        ValueDescriptor *result = nullptr;

        if (leftIsString && rightIsString) {
            const std::string &left = dynamic_cast<StringValueDescriptor *>(leftValue)->value();
            const std::string &right = dynamic_cast<StringValueDescriptor *>(rightValue)->value();

            if (token().isAdditionOperator())
                result = new StringValueDescriptor{left + right};
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
        }
        else {
            if (token().isEqualToOperator())
                result  = new BooleanValueDescriptor{false};
            else if (token().isNotEqualToOperator())
                result = new BooleanValueDescriptor{true};
        }
        delete leftValue;
        delete rightValue;

        if (result != nullptr)
            return result;

        throw std::runtime_error("Invalid operation involving a string");
    }

    const bool leftIsBoolean = leftValue->type() == ValueDescriptor::Type::boolean;
    const bool rightIsBoolean = rightValue->type() == ValueDescriptor::Type::boolean;

    if (leftIsBoolean || rightIsBoolean) {
        ValueDescriptor *result = nullptr;

        if (leftIsBoolean && rightIsBoolean) {
            const bool left = dynamic_cast<BooleanValueDescriptor *>(leftValue)->value();
            const bool right = dynamic_cast<BooleanValueDescriptor *>(rightValue)->value();

            if (token().isEqualToOperator())
                result = new BooleanValueDescriptor{left == right};
            else if (token().isNotEqualToOperator())
                result = new BooleanValueDescriptor{left != right};
        }
        else {
            if (token().isEqualToOperator())
                result = new BooleanValueDescriptor{false};
            else if (token().isNotEqualToOperator())
                result = new BooleanValueDescriptor{true};
        }
        delete leftValue;
        delete rightValue;

        if (result != nullptr)
            return result;
        throw std::runtime_error("Invalid operation involving a boolean");
    }

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
    else if (token().isFloorDivisionOperator()) {
        if (right == 0)
            throw std::runtime_error("Floor division by zero");

        auto quotient = left / right;
        auto remainder = left % right;

        if (remainder != 0 && ((remainder < 0) != (right < 0))) {
            --quotient;
        }
        result = new IntegerValueDescriptor{quotient};
    }
    else if (token().isModuloOperator()) {
        if (right == 0)
            throw std::runtime_error("Modulo by zero");

        auto remainder = left % right;

        if (remainder != 0 && ((remainder < 0) != (right < 0))) {
            remainder += right;
        }
        result = new IntegerValueDescriptor{remainder};
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

IntegerConversion::IntegerConversion(ExprNode *expression) : ExprNode{Token{}}, expression(expression) {}

IntegerConversion::~IntegerConversion() {
    delete expression;
}

void IntegerConversion::print() const {
    std::cout << "int(";
    expression->print();
    std::cout << ")";
}

ValueDescriptor * IntegerConversion::evaluate(const SymbolTable &symbolTable) const {

    ValueDescriptor *value = expression->evaluate(symbolTable);

    if (value->type() == ValueDescriptor::Type::integer) {
        return value;
    }

    if (value->type() == ValueDescriptor::Type::string) {
        delete value;
        throw std::runtime_error("int() requires an integer or string");
    }
    const std::string &text = dynamic_cast<StringValueDescriptor *>(value)->value();
    std::string convertedText = text;

    if (!convertedText.empty() && convertedText.front() == '+') {
        convertedText.erase(0,1);
    }

    if (convertedText.empty()) {
        delete value;
        throw std::runtime_error("invalid integer conversion");
    }

    std::int64_t result{};
    const char *begin = convertedText.data();
    const char *end = begin + convertedText.size();

    auto conversion = std::from_chars(begin, end, result);

    if (conversion.ec != std::errc() || conversion.ptr != end) {
        delete value;
        throw std::runtime_error("invalid integer conversion" + text);
    }
    delete value;
    return new IntegerValueDescriptor{result};
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

StringConversion::StringConversion(ExprNode *expression) : ExprNode{Token{}}, expression(expression){}

StringConversion::~StringConversion() {
    delete expression;
}

void StringConversion::print() const {
    std::cout << "str(";
    expression->print();
    std::cout << ")";
}

ValueDescriptor *StringConversion::evaluate(const SymbolTable &symbolTable) const {

    ValueDescriptor *value = expression->evaluate(symbolTable);

    ValueDescriptor *result = new StringValueDescriptor{value->toString()};

    delete value;
    return result;
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

ListLiteral::ListLiteral(std::vector<ExprNode *> elements) : ExprNode{Token{}}, elements{std::move(elements)} {}

ListLiteral::~ListLiteral() {
    for (ExprNode *element : elements) {
        delete element;
    }
}

void ListLiteral::print() const {
    std::cout << "[";

    for (std::size_t i = 0; i < elements.size(); i++) {
        if (i != 0) {
            std::cout << ",";
        }
        elements[i]->print();
    }
    std::cout << "]";
}

ValueDescriptor *ListLiteral::evaluate(const SymbolTable &) const {
    throw std::runtime_error("not implemented");
}

SubscriptExpression::SubscriptExpression(ExprNode *listExpression, ExprNode *indexExpression) : ExprNode{Token{}},
                                         listExpression{listExpression}, indexExpression{indexExpression} {}

SubscriptExpression::~SubscriptExpression() {
    delete listExpression;
    delete indexExpression;
}

void SubscriptExpression::print() const {
    listExpression->print();
    std::cout << "[";
    indexExpression->print();
    std::cout << "]";
}

ValueDescriptor *SubscriptExpression::evaluate(const SymbolTable &) const {
    throw std::runtime_error("not implemented");
}

FloatConversion::FloatConversion(ExprNode *expression) : ExprNode{Token{}}, expression{expression} {}

FloatConversion::~FloatConversion() {
    delete expression;
}

void FloatConversion::print() const {
    std::cout << "float(";
    expression->print();
    std::cout << ")";
}

ValueDescriptor *FloatConversion::evaluate(const SymbolTable &symbolTable) const {
    ValueDescriptor *value = expression->evaluate(symbolTable);

    if (value->type() == ValueDescriptor::Type::doubleValue) {
        return value;
    }

    if (value->type() == ValueDescriptor::Type::integer) {
        double result = static_cast<double>(dynamic_cast<IntegerValueDescriptor *>(value)->value());
        delete value;
        return new DoubleValueDescriptor{result};
    }

    if (value->type() != ValueDescriptor::Type::string) {
        delete value;
        throw std::runtime_error("float() requires a number or string");
    }

    const std::string &text = dynamic_cast<StringValueDescriptor *>(value)->value();

    static const std::regex validFloat{R"([+-]?[0-9]+(\.[0-9]+)?)"};

    if (!std::regex_match(text, validFloat)) {
        delete value;
        throw std::runtime_error("invalid floating point conversion: " + text);
    }

    try {
        double result = std::stod(text);

        if (!std::isfinite(result)) {
            delete value;
            throw std::runtime_error("floating ppoint value is not finite");
        }

        delete value;
        return new DoubleValueDescriptor{result};

    } catch (const std::exception &) {
        delete value;
        throw std::runtime_error("invalid floating point conversion: " + text);
    }
}

LengthConversion::LengthConversion(ExprNode *expression) : ExprNode(Token{}), expression{expression} {}

LengthConversion::~LengthConversion() {
    delete expression;
}

void LengthConversion::print() const {
    std::cout << "len(";
    expression->print();
    std::cout << ")";
}

ValueDescriptor *LengthConversion::evaluate(const SymbolTable &symbolTable) const {

    ValueDescriptor *value = expression->evaluate(symbolTable);

    if (value->type() == ValueDescriptor::Type::string) {
        auto *stringValue = dynamic_cast<StringValueDescriptor *>(value);
        std::int64_t length = static_cast<std::int64_t>(stringValue->value().size());

        delete value;
        return new IntegerValueDescriptor{length};
    }

    if (value->type() == ValueDescriptor::Type::list) {
        auto *listValue = dynamic_cast<ListValueDescriptor *>(value);
        std::int64_t length = static_cast<std::int64_t>(listValue->object()->size());

        delete value;
        return new IntegerValueDescriptor{length};
    }

    delete value;
    throw std::runtime_error("len() requires a string or list");
}

SplitConversion::SplitConversion(ExprNode *expression) : ExprNode{Token{}}, expression{expression} {}

SplitConversion::~SplitConversion() {
    delete expression;
}

void SplitConversion::print() const {
    std::cout << "split(";
    expression->print();
    std::cout << ")";
}

ValueDescriptor *SplitConversion::evaluate(const SymbolTable &symbolTable) const {

    ValueDescriptor *value = expression->evaluate(symbolTable);

    if (value->type() != ValueDescriptor::Type::string) {
        delete value;
        throw std::runtime_error("split() requires a string");
    }

    std::string text = dynamic_cast<StringValueDescriptor *>(value)->value();

    ListObject *object = new ListObject(ListElementType::string);

    std::size_t start = 0;

    while (start < text.size()) {
        while (start < text.size() && (text[start] == ' ' || text[start] == '\t')) {
            start++;
        }

        if (start == text.size()) {
            break;
        }

        std::size_t end = start;

        while (end < text.size() && (text[end] == ' ' || text[end] == '\t')) {
            end++;
        }
        object->append(new StringValueDescriptor{text.substr(start, end - start)});
        start = end;
    }
    delete value;
    return new ListValueDescriptor{object};
}

PopConversion::PopConversion(ExprNode *expression) : ExprNode{Token{}}, expression{expression} {}

PopConversion::~PopConversion() {
    delete expression;
}

void PopConversion::print() const {
    std::cout << "pop(";
    expression->print();
    std::cout << ")";
}

ValueDescriptor *PopConversion::evaluate(const SymbolTable &symbolTable) const {

    ValueDescriptor *value = expression->evaluate(symbolTable);

    if (value->type() == ValueDescriptor::Type::list) {
        delete value;
        throw std::runtime_error("pop() requires a list");
    }

    auto *listValue = dynamic_cast<ListValueDescriptor *>(value);

    if (listValue->object()->size() == 0) {
        delete value;
        throw std::runtime_error("cannot pop from an empty list");
    }

    ValueDescriptor *result = listValue->object()->pop();
    delete value;
    return result;
}











