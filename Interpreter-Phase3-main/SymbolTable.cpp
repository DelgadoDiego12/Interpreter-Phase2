#include <iostream>
#include <stdexcept>
#include "ValueDescriptor.hpp"
#include "SymbolTable.hpp"

void SymbolTable::setValueFor(const std::string &variableName, const ValueDescriptor &value) {
    values[variableName].reset(value.clone());
}

bool SymbolTable::isDefined(const std::string &variableName) const {
    return values.contains(variableName);
}

const ValueDescriptor &SymbolTable::getValueFor(const std::string &variableName) const {
    const auto entry = values.find(variableName);
    if (entry == values.end())
        throw std::runtime_error("use of undefined variable '" + variableName + "'");
    return *entry->second;
}

void SymbolTable::print() const {
    for (const auto &[variable, value] : values)
        std::cout << variable << " = " << value->toString() << '\n';
}
