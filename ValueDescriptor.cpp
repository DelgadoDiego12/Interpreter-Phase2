#include "ValueDescriptor.hpp"

#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include "SymbolTable.hpp"

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
//
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
//
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
//
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

namespace {
    ListElementType categoryFor(const ValueDescriptor *value) {
        if (value->type() == ValueDescriptor::Type::integer || value->type() == ValueDescriptor::Type::doubleValue) {
            return ListElementType::numeric;
        }
        if (value->type() == ValueDescriptor::Type::string) {
            return ListElementType::string;
        }
        if (value->type() == ValueDescriptor::Type::boolean) {
            return ListElementType::boolean;
        }
        throw std::runtime_error("lists cannot contain lists");
    }

    bool isCompatible(ListElementType category, const ValueDescriptor *value) {
        return category == ListElementType::unset || category == categoryFor(value);
    }
}

ListObject::ListObject(ListElementType initialType) : category{initialType} {}

ListObject::~ListObject() {
    for (ValueDescriptor *value : elements) {
        delete value;
    }
}

void ListObject::retain() {
    ++references;
}

bool ListObject::release() {
    if (references == 0) {
        throw std::runtime_error("list reference count is already zero");
    }
    --references;
    return references == 0;
}

std::size_t ListObject::referenceCount() const {
    return references;
}

std::size_t ListObject::size() const {
    return elements.size();
}

ListElementType ListObject::elementType() const {
    return category;
}

ValueDescriptor *ListObject::cloneElementAt(std::size_t index) const {

    if (index >= elements.size()) {
        throw std::runtime_error("list index out of range");
    }
    return elements[index]->clone();
}

void ListObject::append(ValueDescriptor *value) {

    if (value == nullptr) {
        throw std::runtime_error("cannot append null value");
    }

    if (!isCompatible(category, value)) {
        delete value;
        throw std::runtime_error("list elements must have the same type");
    }

    if (category == ListElementType::unset) {
        category = categoryFor(value);
    }
    elements.push_back(value);
}

ValueDescriptor *ListObject:: pop() {

    if (elements.empty()) {
        throw std::runtime_error("cannot pop from an empty list");
    }

    ValueDescriptor *result = elements.back();
    elements.pop_back();

    return result;
}

ListValueDescriptor::ListValueDescriptor(ListObject *object) : listObject{object} {

    if (listObject == nullptr) {
        throw std::runtime_error("ListValueDescriptor received null object");
    }

    listObject->retain();
}

ListValueDescriptor::~ListValueDescriptor() {
    if (listObject->release()) {
        delete listObject;
    }
}

ValueDescriptor::Type ListValueDescriptor::type() const {
    return ValueDescriptor::Type::list;
}

std::string ListValueDescriptor::typeName() const {
    return "list";
}

std::string ListValueDescriptor::toString() const {
    std::string result = "[";

    for (std::size_t i = 0; i < listObject->size(); ++i) {
        if (i != 0) {
            result += ", ";
        }

        ValueDescriptor *element = listObject->cloneElementAt(i);
        result += element->toString();
        delete element;
    }

    result += "]";
    return result;
}

ValueDescriptor *ListValueDescriptor::clone() const {
    return new ListValueDescriptor(listObject);
}

ListObject *ListValueDescriptor::object() {
    return listObject;
}

const ListObject *ListValueDescriptor::object() const {
    return listObject;
}


