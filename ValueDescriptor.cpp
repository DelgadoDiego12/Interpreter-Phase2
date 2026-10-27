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
