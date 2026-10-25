#include "ValueDescriptor.hpp"
//#inclue <utility>

BooleanValueDescriptor::BooleanValueDescriptor(bool value) : booleanValue(value) {}

ValueDescriptor::Type BooleanValueDescriptor::type() const {
    return Type::boolean;
}

std::string BooleanValueDescriptor::typeName() const {
    return "boolean";
}

std::string BooleanValueDescriptor::toString() const {
    if (booleanValue) { return "True"; }
    return "False";
}

ValueDescriptor *BooleanValueDescriptor::clone() const {
    return new BooleanValueDescriptor(booleanValue);
}

bool BooleanValueDescriptor::value() const {
    return booleanValue;
}

StringValueDescriptor::StringValueDescriptor(std::string value) : stringValue(std::move(value)) {}

ValueDescriptor::Type StringValueDescriptor::type() const {
    return Type::string;
}

std::string StringValueDescriptor::typeName() const {
    return "string";
}

std::string StringValueDescriptor::toString() const {
    return stringValue;
}

ValueDescriptor *StringValueDescriptor::clone() const {
    return new StringValueDescriptor(stringValue);
}

const std::string &StringValueDescriptor::value() const {
    return stringValue;
}