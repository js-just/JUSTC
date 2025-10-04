#include "lexer.h"
#include <stdexcept>
#include <cctype>
#include <algorithm>

Lexer::Lexer(const std::string& input) : input(input), position(0) {
    if (input.empty()) {
        throw std::invalid_argument("Invalid Input.");
    }
    initializeKeywords();
    tokenize();
}

void Lexer::initializeKeywords() {
    keywords = {
        "TYPE", "GLOBAL", "LOCAL", "STRICT", "ALL", "JSON", 
        "HTTPJSON", "HTTPTEXT", "HTTPJUSTC", "JUSTC", 
        "IMPORT", "EXPORT", "COPY", "REQUIRE", "ENV", 
        "CONFIG", "RUN", "VALUE", "FILE", "STAT", "STRING", 
        "LINK", "STRINGNUM", "STRINGB64", "STRINGBIN", 
        "STRINGHEX", "TYPEID", "TYPEOF", "OUTPUT", "RETURN",
        "V", "D", "SQ", "CU", "P", "M", "S", "C", "T", "N",
        "ECHO", "LOGFILE", "LOG", "PARSEJUSTC", "PARSEJSON",
        "SPECIFIED", "EVERYTHING", "DISABLED", "AS",
        "STRINGOCT", "NUMBER", "BINARY", "OCTAL", "BASE64",
        "HEXADECIMAL", "PARSEHOCON", "HTTPHOCON", "BASE32",
        "STRINGB32", "ABSOLUTE", "CEIL", "FLOOR", "ALLOW",
        "DISALLOW", "JAVASCRIPT"
    };
    
    smallkeywords = {
        {"T", "TYPE"}, {"G", "GLOBAL"}, {"L", "LOCAL"}, {"SC", "STRICT"},
        {"A", "ALL"}, {"JS", "JSON"}, {"JC", "JUSTC"}, {"I", "IMPORT"},
        {"EX", "EXPORT"}, {"CP", "COPY"}, {"RQ", "REQUIRE"}, {"E", "ENV"},
        {"HJS", "HTTPJSON"}, {"HT", "HTTPTEXT"}, {"HJ", "HTTPJUSTC"},
        {"HH", "HTTPHOCON"}, {"CO", "CONFIG"}, {"R", "RUN"}, {"O", "VALUE"},
        {"F", "FILE"}, {"FS", "STAT"}, {"ST", "STRING"}, {"SL", "LINK"},
        {"SN", "STRINGNUM"}, {"S64", "STRINGB64"}, {"S02", "STRINGBIN"},
        {"S16", "STRINGHEX"}, {"ID", "TYPEID"}, {"OF", "TYPEOF"},
        {"OUT", "OUTPUT"}, {"RT", "RETURN"}, {"Q", "SQ"}, {"U", "CU"},
        {"PJ", "PARSEJUSTC"}, {"PJS", "PARSEJSON"}, {"SP", "SPECIFIED"},
        {"EV", "EVERYTHING"}, {"N", "DISABLED"}, 
        {"NUM", "NUMBER"}, {"SO", "STRINGOCT"}, {"B", "BINARY"}, 
        {"OC", "OCTAL"}, {"B64", "BASE64"}, {"HEX", "HEXADECIMAL"},
        {"PH", "PARSEHOCON"}, {"S32", "STRINGB32"}, {"B32", "BASE32"},
        {"AB", "ABSOLUTE"}, {"CE", "CEIL"}, {"FL", "FLOOR"}, {"AL", "ALLOW"},
        {"DL", "DISALLOW"}, {"J", "JAVASCRIPT"},
        {"HJC", "HTTPJUSTC"}, {"CF", "CONFIG"}, {"SZ", "STAT"}, 
        {"S10", "STRINGNUM"}, {"SNUM", "STRINGNUM"}, {"TID", "TYPEID"},
        {"TO", "TYPEOF"}, {"PUT", "RETURN"}, {"PJC", "PARSEJUSTC"},
        {"SPEC", "SPECIFIED"}, {"EVERY", "EVERYTHING"}, {"DIS", "DISABLED"},
        {"SOCT", "STRINGOCT"}, {"S08", "STRINGOCT"}, {"BIN", "BINARY"},
        {"OCT", "OCTAL"}, {"HEXDEC", "HEXADECIMAL"}, {"B02", "BINARY"},
        {"B08", "OCTAL"}, {"B10", "NUMBER"}, {"B16", "HEXADECIMAL"},
        {"ABS", "ABSOLUTE"}
    };
    
    bigkeywords = {
        {"SQRT", "V"}, {"ROOT", "V"}, {"DOUBLE", "D"}, {"SQUARE", "SQ"},
        {"CUBE", "CU"}, {"NEGATIVE", "N"}, {"SIN", "S"}, {"COS", "C"},
        {"TAN", "T"}, {"SINE", "S"}, {"COSINE", "C"}, {"TANGENT", "T"}
    };
    
    for (const auto& pair : smallkeywords) {
        skw.push_back(pair.first);
    }
    
    for (const auto& pair : bigkeywords) {
        bkw.push_back(pair.first);
    }
}

bool Lexer::isWhitespace(char ch) const {
    return std::isspace(static_cast<unsigned char>(ch));
}

bool Lexer::isLetter(char ch) const {
    return std::isalpha(static_cast<unsigned char>(ch)) || ch == '_';
}

bool Lexer::isDigit(char ch) const {
    return std::isdigit(static_cast<unsigned char>(ch));
}

bool Lexer::isHexDigit(char ch) const {
    return std::isxdigit(static_cast<unsigned char>(ch));
}

bool Lexer::isBase64Char(char ch) const {
    return std::isalnum(static_cast<unsigned char>(ch)) || ch == '+' || ch == '/' || ch == '=';
}

char Lexer::peek(size_t offset) const {
    if (position + offset < input.length()) {
        return input[position + offset];
    }
    return '\0';
}

void Lexer::readComment() {
    while (position < input.length() && input[position] != '\n') {
        position++;
    }
    if (position < input.length() && input[position] == '\n') {
        position++;
    }
}

Token Lexer::readString() {
    size_t start = ++position;
    std::string value;
    while (position < input.length() && 
           (input[position] != '"' || 
           (input[position] == '"' && position > 0 && input[position - 1] == '\\'))) {
        value += input[position++];
    }
    position++;
    return Token("string", value, start);
}

Token Lexer::readAngleString() {
    size_t start = ++position;
    std::string value;
    while (position < input.length() && input[position] != '>') {
        value += input[position++];
    }
    position++;
    return Token("link", value, start);
}

Token Lexer::readNumber() {
    size_t start = position;
    bool point = false;
    
    while (position < input.length() && 
           (isDigit(input[position]) ||
            std::string(".#&bB").find(input[position]) != std::string::npos ||
            isHexDigit(input[position]) || 
            isBase64Char(input[position]))) {
        
        char ch = input[position];
        if ((std::isalnum(static_cast<unsigned char>(ch)) || ch == '.' ||
             ch == '#' || ch == '&' || ch == 'b' || ch == 'B') &&
            ((ch == '.' && position + 1 < input.length() && 
              isDigit(input[position + 1]) && !point) || ch != '.')) {
            
            position++;
            if (ch == '.') {
                point = true;
            }
        } else {
            break;
        }
    }
    
    std::string numStr = input.substr(start, position - start);
    std::string checkstr = numStr;
    std::transform(checkstr.begin(), checkstr.end(), checkstr.begin(), ::tolower);
    
    std::string type;
    if (checkstr[0] == '#') {
        type = "hex";
    } else if (checkstr[0] == '&') {
        type = "base64";
    } else if (checkstr[0] == 'b') {
        type = "binary";
    } else {
        type = "number";
    }
    
    return Token(type, numStr, start);
}

Token Lexer::readIdentifier() {
    size_t start = position;
    while (position < input.length() && 
           (isLetter(input[position]) || 
            isDigit(input[position]) || 
            input[position] == '\'')) {
        position++;
    }
    
    std::string id = input.substr(start, position - start);
    
    if (std::find(keywords.begin(), keywords.end(), id) != keywords.end()) {
        return Token("keyword", id, start);
    } else if (smallkeywords.find(id) != smallkeywords.end()) {
        return Token("keyword", smallkeywords[id], start);
    } else if (bigkeywords.find(id) != bigkeywords.end()) {
        return Token("keyword", bigkeywords[id], start);
    }
    
    if (id == "is" || id == "isn't" || id == "isif" || id == "then" || 
        id == "elseif" || id == "else" || id == "isifn't" || id == "elseifn't" || 
        id == "then't" || id == "elsen't" || id == "or" || id == "orn't") {
        return Token("keyword", id, start);
    }
    
    std::string idLower = id;
    std::transform(idLower.begin(), idLower.end(), idLower.begin(), ::tolower);
    if (idLower == "true" || idLower == "yes" || idLower == "false" || idLower == "no") {
        return Token("boolean", id, start);
    }
    
    if (idLower == "null" || idLower == "nil") {
        return Token("null", id, start);
    }
    
    if (idLower == "undefined") {
        return Token("undefined", id, start);
    }
    
    return Token("identifier", id, start);
}

void Lexer::tokenize() {
    while (position < input.length()) {
        char ch = input[position];
        
        if (isWhitespace(ch)) {
            position++;
            continue;
        }
        
        if (ch == '-' && peek() == '-') {
            readComment();
            continue;
        }
        
        if (ch == '"') {
            tokens.push_back(readString());
            continue;
        }
        
        if (ch == '<') {
            tokens.push_back(readAngleString());
            continue;
        }
        
        if (ch == '.' && peek() == '.') {
            tokens.push_back(Token("..", "..", position));
            position += 2;
            continue;
        }
        
        if (isDigit(ch)) {
            tokens.push_back(readNumber());
            continue;
        }
        
        if (ch == ',' || ch == '.' || ch == '[' || ch == ']') {
            tokens.push_back(Token(std::string(1, ch), std::string(1, ch), position));
            position++;
            continue;
        }
        
        if (ch == '-') {
            tokens.push_back(Token("minus", "-", position));
            position++;
            continue;
        }
        
        if (isLetter(ch)) {
            tokens.push_back(readIdentifier());
            continue;
        }
        
        tokens.push_back(Token(std::string(1, ch), std::string(1, ch), position));
        position++;
    }
}

std::vector<Token> Lexer::getTokens() const {
    return tokens;
}

std::pair<std::string, std::vector<Token>> Lexer::parse(const std::string& input) {
    Lexer lexer(input);
    return std::make_pair(input, lexer.getTokens());
}
