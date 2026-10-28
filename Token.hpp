#ifndef ALEXICALANALYZER_TOKEN_HPP
#define ALEXICALANALYZER_TOKEN_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include <utility>
#include <cstdint>

enum class Keyword {
    none,
    forKeyword,
    printKeyword,
    inKeyword,
    rangeKeyword,
    trueKeyword,
    falseKeyword,
    strKeyword
};

class Token {
public:
    [[nodiscard]] bool isEof() const { return _eof; }
    [[nodiscard]] bool isNewline() const { return _newline; }
    void markAsEof() { _eof = true; }
    void markAsNewline() { _newline = true; }

    void setLocation(std::size_t line, std::size_t column) {
        _lineNumber = line;
        _columnNumber = column;
    }
    [[nodiscard]] std::size_t lineNumber() const { return _lineNumber; }
    [[nodiscard]] std::size_t columnNumber() const { return _columnNumber; }

    void setSymbol(std::string symbol) { _symbol = symbol; }
    [[nodiscard]] std::string symbol() const { return _symbol; }

    [[nodiscard]] bool isOpenParen() const { return _symbol == "("; }
    [[nodiscard]] bool isCloseParen() const { return _symbol == ")"; }
    [[nodiscard]] bool isSemicolon() const { return _symbol == ";"; }
    [[nodiscard]] bool isAssignmentOperator() const { return _symbol == "="; }
    [[nodiscard]] bool isMultiplicationOperator() const { return _symbol == "*"; }
    [[nodiscard]] bool isAdditionOperator() const { return _symbol == "+"; }
    [[nodiscard]] bool isSubtractionOperator() const { return _symbol == "-"; }
    [[nodiscard]] bool isModuloOperator() const { return _symbol == "%"; }
    [[nodiscard]] bool isDivisionOperator() const { return _symbol == "/"; }
    [[nodiscard]] bool isFloorDivisionOperator() const { return _symbol == "//"; }

    [[nodiscard]] bool isLessThanOperator() const { return _symbol == "<"; }
    [[nodiscard]] bool isGreaterThanOperator() const { return _symbol == ">"; }
    [[nodiscard]] bool isOpenCurlyBrace() const { return _symbol == "{"; }
    [[nodiscard]] bool isCloseCurlyBrace() const { return _symbol == "}"; }
    [[nodiscard]] bool isEqualToOperator() const { return _symbol == "=="; }
    [[nodiscard]] bool isNotEqualToOperator() const { return _symbol == "!="; }
    [[nodiscard]] bool isLessOrEqualToOperator() const { return _symbol == "<="; }
    [[nodiscard]] bool isGreaterOrEqualToOperator() const { return _symbol == ">="; }
    [[nodiscard]] bool isQuotationMark() const { return _symbol == "\""; } //for strings
    [[nodiscard]] bool isSingleQuotMark() const { return _symbol == "\'"; } //for string
    [[nodiscard]] bool isColon() const { return _symbol == ":"; }
    [[nodiscard]] bool isComma() const { return _symbol == ","; }
    [[nodiscard]] bool isIndent() const { return _indent; }
    [[nodiscard]] bool isDedent() const { return _dedent; }
    void markAsIndent() { _indent = true; }
    void markAsDedent() { _dedent = true;}



    void setIdentifier(std::string identifier) { _identifier = std::move(identifier); }
    [[nodiscard]] bool isIdentifier() const { return !_identifier.empty(); }
    [[nodiscard]] const std::string &identifier() const { return _identifier; }

    void setKeyword(Keyword keyword) { _keyword = keyword; }
    [[nodiscard]] bool isKeyword() const { return _keyword != Keyword::none; }
    [[nodiscard]] bool isForKeyword() const { return _keyword == Keyword::forKeyword; }
    [[nodiscard]] bool isPrintKeyword() const { return _keyword == Keyword::printKeyword; }
    [[nodiscard]] bool isInKeyword() const { return _keyword == Keyword::inKeyword; } 
    [[nodiscard]] bool isRangeKeyword() const { return _keyword == Keyword::rangeKeyword; } 
    [[nodiscard]] bool isTrueKeyword() const { return _keyword == Keyword::trueKeyword; } //
    [[nodiscard]] bool isFalseKeyword() const { return _keyword == Keyword::falseKeyword; } //
    [[nodiscard]] bool isSTRKeyword() const {return _keyword == Keyword::strKeyword; }
    [[nodiscard]] Keyword keyword() const { return _keyword; }

    void setIntegerValue(std::int64_t value) {
        _integerValue = value;
        _isInteger = true;
    }
    [[nodiscard]] bool isInteger() const { return _isInteger; }
    [[nodiscard]] std::int64_t integerValue() const { return _integerValue; }
    
    void setDoubleValue(double value) {
        _doubleValue = value;
        _isDouble = true;
    }
    [[nodiscard]] bool isDouble() const { return _isDouble; }
    [[nodiscard]] double doubleValue() const { return _doubleValue; }

    void setStringValue(std::string value) {
        _stringValue = std::move(value);
        _isString = true;
    }
    [[nodiscard]] bool isString() const { return _isString; }
    [[nodiscard]] const std::string &stringValue() const { return _stringValue; }

    void print(std::ostream &output) const;

private:
    std::string _identifier{};
    Keyword _keyword{Keyword::none};
    bool _eof{false};
    bool _newline{false};
    bool _isInteger{false};
    bool _isDouble{false};
    bool _isString{false};
    bool _indent{false};
    bool _dedent{false};
    std::string _symbol{'\0'};
    std::int64_t _integerValue{0};
    std::string _stringValue{};
    double _doubleValue{0.0};
    std::size_t _lineNumber{0};
    std::size_t _columnNumber{0};
};

#endif // ALEXICALANALYZER_TOKEN_HPP
