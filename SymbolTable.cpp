#include <iostream>
#include <stdexcept>

#include "SymbolTable.hpp"

#include "Token.hpp"

SymbolTable::~SymbolTable() {
    for (auto &[vairable, value] : values)
        delete value;
}

void SymbolTable::setValueFor(const std::string &variableName, ValueDescriptor *value) {
    auto existing = values.find(variableName);

    if (existing != values.end()) {
        delete existing->second;
    }

    values[variableName] = value;
}


bool SymbolTable::isDefined(const std::string &variableName) const {
    return values.contains(variableName);
}

const ValueDescriptor *SymbolTable::getValueFor(const std::string &variableName) const {
    const auto entry = values.find(variableName);

    if (entry == values.end()) {
        throw std::runtime_error("use of undefined variable '" + variableName + "'");
    }
    return entry->second;
}

void SymbolTable::print() const {
    for (const auto &[variable, value] : values)
        std::cout << variable << " = " << value->toString() << std::endl;
}
