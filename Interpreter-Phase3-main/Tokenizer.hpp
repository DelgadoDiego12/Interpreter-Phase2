#ifndef ALEXICALANALYZER_TOKENIZER_HPP
#define ALEXICALANALYZER_TOKENIZER_HPP

#include <cstddef>
#include <fstream>
#include <iosfwd>
#include <string>
#include <vector>

#include "Token.hpp"

class Tokenizer {
public:
    explicit Tokenizer(std::ifstream &inputStream);

    Token getToken();
    void ungetToken();
    void printProcessedTokens(std::ostream &output) const;

private:
    Token lastToken{};
    bool ungottenToken{false};
    std::ifstream &inputStream;
    std::vector<Token> tokens{};
    std::size_t lineNumber{1};
    std::size_t columnNumber{1};
    bool lineContainsToken{false};

    bool atBeginningOfLine{true};
    bool pendingIndent{false};
    int pendingDedents{0};
    std::vector<std::size_t> indentationLevels{0};
    void processBeginningOfLine();

    bool getCharacter(char &character);
    std::string readIdentifier(char firstCharacter);
    int readInteger(char firstDigit);

    static bool isDigit(char character);
    static bool isIdentifierStart(char character);
    static bool isIdentifierPart(char character);
    static bool isDiscardedWhitespace(char character);
    bool isValidStringChar(char character);
};

#endif // ALEXICALANALYZER_TOKENIZER_HPP