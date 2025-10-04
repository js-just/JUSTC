#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct Token {
    std::string type;
    std::string value;
    size_t start;
    
    Token(const std::string& t, const std::string& v, size_t s) 
        : type(t), value(v), start(s) {}
};

class Lexer {
private:
    std::string input;
    size_t position;
    std::vector<Token> tokens;
    
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
    Token readString();
    Token readAngleString();
    Token readNumber();
    Token readIdentifier();
    void tokenize();

public:
    Lexer(const std::string& input);
    std::vector<Token> getTokens() const;
    static std::pair<std::string, std::vector<Token>> parse(const std::string& input);
};

#endif // LEXER_H
