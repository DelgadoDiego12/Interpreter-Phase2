#include <cctype>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

#include "Tokenizer.hpp"

bool Tokenizer::isDigit(char character) {
    return std::isdigit(static_cast<unsigned char>(character)) != 0;
}

bool Tokenizer::isIdentifierStart(char character) {
    return std::isalpha(static_cast<unsigned char>(character)) != 0 || character == '_';
}

bool Tokenizer::isIdentifierPart(char character) {
    return std::isalnum(static_cast<unsigned char>(character)) != 0 || character == '_';
}

bool Tokenizer::isDiscardedWhitespace(char character) {
    return character != '\n' && std::isspace(static_cast<unsigned char>(character)) != 0;
}

bool Tokenizer::isValidStringChar(char character) {
    unsigned char c = static_cast<unsigned char>(character);
    if (c >= 32 && c <= 126 && c != '\\') {
        return true;
    }
    return false;
}

bool Tokenizer::getCharacter(char &character) {
    if (!inputStream.get(character))
        return false;

    if (character == '\n') {
        ++lineNumber;
        columnNumber = 1;
    } else {
        ++columnNumber;
    }
    return true;
}

std::string Tokenizer::readIdentifier(char firstCharacter) {
    std::string identifier{firstCharacter};
    while (inputStream.peek() != std::char_traits<char>::eof()) {
        char character = static_cast<char>(inputStream.peek());
        if (!isIdentifierPart(character))
            break;
        getCharacter(character);
        identifier += character;
    }
    return identifier;
}

int Tokenizer::readInteger(char firstDigit) {
    int value = firstDigit - '0';
    while (inputStream.peek() != std::char_traits<char>::eof()) {
        char character = static_cast<char>(inputStream.peek());
        if (!isDigit(character))
            break;

        const int digit = character - '0';
        if (value > (std::numeric_limits<int>::max() - digit) / 10) {
            std::cerr << "Integer literal is too large at line " << lineNumber
                      << ", column " << columnNumber << ".\n";
            std::exit(EXIT_FAILURE);
        }

        getCharacter(character);
        value = value * 10 + digit;
    }
    return value;
}

Tokenizer::Tokenizer(std::ifstream &stream) : inputStream{stream} {}

void Tokenizer::processBeginningOfLine() {
    while (atBeginningOfLine && inputStream.peek() != std::char_traits<char>::eof()) {
        std::size_t spaces = 0;

        while (inputStream.peek() == ' ') {
            char character;
            getCharacter(character);
            ++spaces;
        }
        if (inputStream.peek() == '\t') {
            std::cerr << "Tabs are not permitted as indentations at line " << lineNumber << ".\n";
            std::exit(EXIT_FAILURE);
        }
        if (inputStream.peek() == '\n') {
            char character;
            getCharacter(character);
            continue;
        }
        if (inputStream.peek() == std::char_traits<char>::eof()) {
            return;
        }
        std::size_t currentIndent = indentationLevels.back();

        if (spaces > currentIndent) {
            indentationLevels.push_back(spaces);
            pendingIndent = true;
        } else if (spaces < currentIndent) {
            while (indentationLevels.size() > 1 && spaces < indentationLevels.back()) {
                indentationLevels.pop_back();
                ++pendingDedents;
            }
            if (spaces != indentationLevels.back()) {
                std::cerr << "Invalid indentation at line " << lineNumber << ", indentation does not match a previous level.\n";
                std::exit(EXIT_FAILURE);
            }
        }
        atBeginningOfLine = false;
    }
}

Token Tokenizer::getToken() {
    if (ungottenToken) {
        ungottenToken = false;
        return lastToken;
    }

    if (pendingIndent) {
        pendingIndent = false;
        Token token;
        token.setLocation(lineNumber, 1);
        token.markAsIndent();
        tokens.push_back(token);
        return lastToken = token;
    }

    if (pendingDedents > 0) {
        --pendingDedents;
        Token token;
        token.setLocation(lineNumber, 1);
        token.markAsDedent();
        tokens.push_back(token);
        return lastToken = token;
    }

    if (atBeginningOfLine) {
        processBeginningOfLine();

        if (pendingIndent) {
            pendingIndent = false;
            Token token;
            token.setLocation(lineNumber, 1);
            token.markAsIndent();
            tokens.push_back(token);
            return lastToken = token;
        }

        if (pendingDedents > 0) {
            --pendingDedents;
            Token token;
            token.setLocation(lineNumber, 1);
            token.markAsDedent();
            tokens.push_back(token);
            return lastToken = token;
        }
    }

    while (inputStream.peek() != std::char_traits<char>::eof()) {
        char character = static_cast<char>(inputStream.peek());

        if (isDiscardedWhitespace(character)) {
            getCharacter(character);
            continue;
        }

        if (character == '\n') {
            const auto newlineLine = lineNumber;
            const auto newlineColumn = columnNumber;
            getCharacter(character);

            if (lineContainsToken) {
                Token token;
                token.setLocation(newlineLine, newlineColumn);
                token.markAsNewline();
                lineContainsToken = false;
                atBeginningOfLine = true;
                tokens.push_back(token);
                return lastToken = token;
            }
            atBeginningOfLine = true;
            processBeginningOfLine();

            if (pendingIndent) {
                pendingIndent = false;
                Token token;
                token.setLocation(lineNumber, 1);
                token.markAsIndent();
                tokens.push_back(token);
                return lastToken = token;
            }
            if (pendingDedents > 0) {
                --pendingDedents;
                Token token;
                token.setLocation(lineNumber, 1);
                token.markAsDedent();
                tokens.push_back(token);
                return lastToken = token;
            }
            continue;
        }

        break;
    }

    if (inputStream.peek() == std::char_traits<char>::eof()) {
        if (inputStream.bad()) {
            std::cerr << "Error while reading the input stream in Tokenizer.\n";
            std::exit(EXIT_FAILURE);
        }
        if (lineContainsToken) {
            Token token;
            token.setLocation(lineNumber, columnNumber);
            token.markAsNewline();
            lineContainsToken = false;
            tokens.push_back(token);
            return lastToken = token;
        }
        if (indentationLevels.size() > 1) {
            indentationLevels.pop_back();
            Token token;
            token.setLocation(lineNumber, columnNumber);
            token.markAsDedent();
            tokens.push_back(token);
            return lastToken = token;
        }

        Token token;
        token.setLocation(lineNumber, columnNumber);
        token.markAsEof();

        tokens.push_back(token);
        return lastToken = token;
    }
    Token token;
    token.setLocation(lineNumber, columnNumber);

    char character;
    getCharacter(character);
    char nextChar = static_cast<char>(inputStream.peek());

    if (isDigit(character)) {
        token.setIntegerValue(readInteger(character));
    }
    //-
    else if (character == '\"' || character == '\'') {
        char quote = character;
        char next = inputStream.peek();
        std::string value;
        while(next != std::char_traits<char>::eof() && next != quote) {
            if (isValidStringChar(character)) {
                value += character;
            }
            else {
                std::cerr << "Invalid character";
                std::exit(EXIT_FAILURE);
            }
        }
        if (next == quote) {
            token.setStringValue(std::move(value));
        }
    }
    else if ((character == '=' || character == '!' || character == '<' || character == '>') && (nextChar == '=')){
        getCharacter(nextChar);
        std::string twoChar = ""; //creating string for 2 character operators
        twoChar = character;
        twoChar += nextChar;
        token.setSymbol(twoChar);
    }
    else if (character == '=' || character == '+' || character == '-' ||
            character == '*' || character == '/' || character == '%' ||
            character == ';' || character == '(' || character == ')' ||
            character == '>' || character == '<' || character == '{' ||
            character == '}' || character == ':' || character == ',') {
        std::string oneChar = "";
        oneChar = character; //to convert char to string
        token.setSymbol(oneChar);
            }
    else if (isIdentifierStart(character)) {
        std::string identifier = readIdentifier(character);
        if (identifier == "for")
            token.setKeyword(Keyword::forKeyword);
        else if (identifier == "print")
            token.setKeyword(Keyword::printKeyword);
        else if (identifier == "in")
            token.setKeyword(Keyword::inKeyword);
        else if (identifier == "range")
            token.setKeyword(Keyword::rangeKeyword);
        else if (identifier == "True")
            token.setKeyword(Keyword::trueKeyword);
        else if (identifier == "False")
            token.setKeyword(Keyword::falseKeyword);
        else
            token.setIdentifier(std::move(identifier));
    } else {
        std::cerr << "Unknown character in input at line " << token.lineNumber()
                  << ", column " << token.columnNumber() << ": '"
                  << character << "'.\n";
        std::exit(EXIT_FAILURE);
    }

    lineContainsToken = true;
    tokens.push_back(token);
    return lastToken = token;
}

void Tokenizer::ungetToken() {
    if (ungottenToken) {
        std::cerr << "Tokenizer supports only one ungotten token.\n";
        std::exit(EXIT_FAILURE);
    }
    ungottenToken = true;
}

void Tokenizer::printProcessedTokens(std::ostream &output) const {
    for (const auto &token : tokens) {
        token.print(output);
        output << '\n';
    }
}
