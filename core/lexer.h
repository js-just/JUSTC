/*

MIT License

Copyright (c) 2025 JustStudio. <https://juststudio.is-a.dev/>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "keywords.h"

struct ParserToken {
    std::string type;
    std::string value;
    size_t start;

    ParserToken() : type(""), value(""), start(0) {}
    ParserToken(const std::string& t, const std::string& v, size_t s) 
        : type(t), value(v), start(s) {}
};

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

    bool dollarBefore;
    void addDollarBefore();

public:
    Lexer(const std::string& input);
    std::vector<ParserToken> getTokens() const;
    static std::pair<std::string, std::vector<ParserToken>> parse(const std::string& input);
};

#endif // LEXER_H
