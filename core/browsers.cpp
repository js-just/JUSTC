#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "json_serializer.h"

extern "C" {

namespace {
bool parseJsonTokens(const char* tokensJson, std::vector<ParserToken>& parserTokens) {
    if (!tokensJson) return false;
    
    std::string jsonStr(tokensJson);
    
    size_t tokensStart = jsonStr.find("\"tokens\":[");
    if (tokensStart == std::string::npos) return false;
    
    size_t pos = tokensStart + 10;
    
    while (pos < jsonStr.length()) {
        if (jsonStr[pos] == '{') {
            ParserToken token;
            size_t tokenEnd = jsonStr.find('}', pos);
            if (tokenEnd == std::string::npos) break;
            
            std::string tokenStr = jsonStr.substr(pos, tokenEnd - pos + 1);
            
            size_t typeStart = tokenStr.find("\"type\":\"");
            if (typeStart != std::string::npos) {
                typeStart += 8;
                size_t typeEnd = tokenStr.find('"', typeStart);
                if (typeEnd != std::string::npos) {
                    token.type = tokenStr.substr(typeStart, typeEnd - typeStart);
                }
            }
            
            size_t valueStart = tokenStr.find("\"value\":\"");
            if (valueStart != std::string::npos) {
                valueStart += 9;
                size_t valueEnd = tokenStr.find('"', valueStart);
                if (valueEnd != std::string::npos) {
                    token.value = tokenStr.substr(valueStart, valueEnd - valueStart);
                }
            }
            
            size_t startStart = tokenStr.find("\"start\":");
            if (startStart != std::string::npos) {
                startStart += 8;
                size_t startEnd = tokenStr.find_first_of(",}", startStart);
                if (startEnd != std::string::npos) {
                    std::string startStr = tokenStr.substr(startStart, startEnd - startStart);
                    token.start = std::atoi(startStr.c_str());
                }
            }
            
            if (!token.type.empty()) {
                parserTokens.push_back(token);
            }
            
            pos = tokenEnd + 1;
        } else if (jsonStr[pos] == ']') {
            break;
        } else {
            pos++;
        }
    }
    
    return !parserTokens.empty();
}
} // namespace

char* lexer(const char* input) {
    if (input == nullptr) return nullptr;
    
    try {
        auto parsed = Lexer::parse(input);
        std::string json = JsonSerializer::serialize(parsed.second, parsed.first);
        return strdup(json.c_str());
        
    } catch (const std::exception& e) {
        std::string error = "{\"error\":\"" + std::string(e.what()) + "\"}";
        return strdup(error.c_str());
    }
}

char* parser(const char* tokensJson) {
    if (tokensJson == nullptr) return nullptr;
    
    try {
        std::vector<ParserToken> parserTokens;
        
        if (parseJsonTokens(tokensJson, parserTokens)) {
            ParseResult result = Parser::parseTokens(parserTokens);
            std::string json = JsonSerializer::serialize(result);
            return strdup(json.c_str());
        } else {
            std::string error = "{\"error\":\"Failed to parse tokens JSON\"}";
            return strdup(error.c_str());
        }
        
    } catch (const std::exception& e) {
        std::string error = "{\"error\":\"" + std::string(e.what()) + "\"}";
        return strdup(error.c_str());
    }
}

char* parse(const char* input) {
    if (input == nullptr) return nullptr;
    
    try {
        auto lexerResult = Lexer::parse(input);
        ParseResult result = Parser::parseTokens(lexerResult.second);
        std::string json = JsonSerializer::serialize(result);
        return strdup(json.c_str());
        
    } catch (const std::exception& e) {
        std::string error = "{\"error\":\"" + std::string(e.what()) + "\"}";
        return strdup(error.c_str());
    }
}

void free_string(char* str) {
    if (str != nullptr) {
        free(str);
    }
}

}
