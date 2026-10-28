#ifndef EXPRINTER_SYMBOLTABLE_HPP
#define EXPRINTER_SYMBOLTABLE_HPP

#include <map>
#include <string>
#include "ValueDescriptor.hpp"

class SymbolTable {
public:
    SymbolTable() = default;
    ~SymbolTable();

    SymbolTable(const SymbolTable &) = delete;
    SymbolTable &operator=(const SymbolTable &) = delete;
    SymbolTable(SymbolTable &&) = delete;
    SymbolTable &operator=(SymbolTable &&) = delete;

    void setValueFor(const std::string &variableName, ValueDescriptor *value);

    [[nodiscard]] bool isDefined(const std::string &variableName) const;
    [[nodiscard]] const ValueDescriptor *getValueFor(const std::string &variableName) const;

    void print() const;

private:
    std::map<std::string, ValueDescriptor *> values;
};

#endif // EXPRINTER_SYMBOLTABLE_HPP
