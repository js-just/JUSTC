#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "parser.h"
#include "keywords.h"

class Lexer {
private:
    std::string input;
    size_t position;
    std::vector<ParserToken> tokens;
    
    std::vector<std::string> keywords;
    std::unordered_map<std::string, std::string> smallkeywords;
    std::unordered_map<std::string, std::string> bigkeywords;
    std::vector<std::string> skw;
    std::vector<std::string> bkw;
    
    void initializeKeywords();
    bool isWhitespace(char ch) const;
    bool isLetter(char ch) const;
    bool isDigit(char ch) const;
    bool isHexDigit(char ch) const;
    bool isBase64Char(char ch) const;
    char peek(size_t offset = 1) const;
    void readComment();
    ParserToken readString();
    ParserToken readAngleString();
    ParserToken readNumber();
    ParserToken readIdentifier();
    void tokenize();

    void invalidInput();
    void invalidUsage();

public:
    Lexer(const std::string& input);
    std::vector<ParserToken> getTokens() const;
    static std::pair<std::string, std::vector<ParserToken>> parse(const std::string& input);
};

#endif // LEXER_H
