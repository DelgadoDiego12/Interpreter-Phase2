#include <sstream>
#include "ValueDescriptor.hpp"

#include "SymbolTable.hpp"

DoubleValueDescriptor::DoubleValueDescriptor(double value) : doubleValue{value} {}

ValueDescriptor::Type DoubleValueDescriptor::type() const {
    return Type::doubleValue;
}

std::string DoubleValueDescriptor::typeName() const {
    return "double";
}

std::string DoubleValueDescriptor::toString() const {
    std::ostringstream output;
    output << doubleValue;
    return output.str();
}

ValueDescriptor * DoubleValueDescriptor::clone() const {
    return new DoubleValueDescriptor(doubleValue);
}

double DoubleValueDescriptor::value() const {
    return doubleValue;
}

IntegerValueDescriptor::IntegerValueDescriptor(std::int64_t value) : integerValue{value} {}

ValueDescriptor::Type IntegerValueDescriptor::type() const {
    return Type::integer;
}

std::string IntegerValueDescriptor::typeName() const {
    return "integer";
}

std::string IntegerValueDescriptor::toString() const {
    return std::to_string(integerValue);
}

ValueDescriptor * IntegerValueDescriptor::clone() const {
    return new IntegerValueDescriptor{integerValue};
}

std::int64_t IntegerValueDescriptor::value() const {
    return integerValue;
}