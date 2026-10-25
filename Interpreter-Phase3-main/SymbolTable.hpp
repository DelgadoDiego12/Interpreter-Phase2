#ifndef EXPRINTER_SYMBOLTABLE_HPP
#define EXPRINTER_SYMBOLTABLE_HPP
#include <memory>
#include <map>
#include <string>
#include "ValueDescriptor.hpp"

class SymbolTable {
public:
    void setValueFor(const std::string &variableName, const ValueDescriptor &value);
    [[nodiscard]] bool isDefined(const std::string &variableName) const;
    [[nodiscard]] const ValueDescriptor &getValueFor(const std::string &variableName) const;
    void print() const;

private:
    std::map<std::string, std::unique_ptr<ValueDescriptor>> values;
};

#endif // EXPRINTER_SYMBOLTABLE_HPP
