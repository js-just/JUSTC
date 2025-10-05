#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include "lexer.h"
#include "parser.h"

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

char* tokensToJson(const std::vector<Token>& tokens, const std::string& input) {
    std::stringstream json;
    json << "{";
    json << "\"input\":\"";
    
    for (char c : input) {
        if (c == '"') json << "\\\"";
        else if (c == '\\') json << "\\\\";
        else if (c == '\n') json << "\\n";
        else if (c == '\t') json << "\\t";
        else if (c == '\r') json << "\\r";
        else json << c;
    }
    
    json << "\",";
    json << "\"tokens\":[";
    
    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& token = tokens[i];
        
        json << "{";
        json << "\"type\":\"";
        json << token.type;
        json << "\",";
        json << "\"value\":\"";
        json << token.value;
        json << "\",";
        json << "\"start\":" << token.start;
        json << "}";
        
        if (i < tokens.size() - 1) {
            json << ",";
        }
    }
    
    json << "]}";
    
    return strdup(json.str().c_str());
}

char* lexer(const char* input) {
    if (input == nullptr) return nullptr;
    
    try {
        auto parsed = Lexer::parse(input);
        return tokensToJson(parsed.second, parsed.first);
        
    } catch (const std::exception& e) {
        std::string error = "{\"error\":\"";
        error += e.what();
        error += "\"}";
        return strdup(error.c_str());
    }
}

char* parser(const char* tokensJson) {
    if (tokensJson == nullptr) return nullptr;
    
    try {
        std::vector<ParserToken> parserTokens;
        
        if (parseJsonTokens(tokensJson, parserTokens)) {
            std::string result = Parser::parseTokens(parserTokens);
            return strdup(result.c_str());
        } else {
            std::string error = "{\"error\":\"Failed to parse tokens JSON\"}";
            return strdup(error.c_str());
        }
        
    } catch (const std::exception& e) {
        std::string error = "{\"error\":\"";
        error += e.what();
        error += "\"}";
        return strdup(error.c_str());
    }
}

char* parse(const char* input) {
    if (input == nullptr) return nullptr;
    
    try {
        auto lexerResult = Lexer::parse(input);
        
        std::vector<ParserToken> parserTokens;
        for (const auto& token : lexerResult.second) {
            parserTokens.push_back({token.type, token.value, token.start});
        }
        
        std::string result = Parser::parseTokens(parserTokens);
        
        return strdup(result.c_str());
        
    } catch (const std::exception& e) {
        std::string error = "{\"error\":\"";
        error += e.what();
        error += "\"}";
        return strdup(error.c_str());
    }
}

void free_string(char* str) {
    if (str != nullptr) {
        free(str);
    }
}

}
