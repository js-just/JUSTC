#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include "lexer.h"
#include "parser.h"

extern "C" {

namespace {
bool parseJsonTokens(const char* tokensJson, std::vector<ParserToken>& parserTokens) {
    if (!tokensJson) return false;
    
    std::string jsonStr(tokensJson);
    size_t pos = 0;
    
    size_t tokensStart = jsonStr.find("\"tokens\":[");
    if (tokensStart == std::string::npos) return false;
    pos = tokensStart + 10;
    
    while (pos < jsonStr.length()) {
        size_t objStart = jsonStr.find('{', pos);
        if (objStart == std::string::npos) break;
        
        size_t objEnd = jsonStr.find('}', objStart);
        if (objEnd == std::string::npos) break;
        
        std::string tokenObj = jsonStr.substr(objStart, objEnd - objStart + 1);
        ParserToken token;
        
        size_t typeStart = tokenObj.find("\"type\":\"");
        if (typeStart != std::string::npos) {
            typeStart += 8;
            size_t typeEnd = tokenObj.find('"', typeStart);
            if (typeEnd != std::string::npos) {
                token.type = tokenObj.substr(typeStart, typeEnd - typeStart);
                
                std::string decodedType;
                for (size_t i = 0; i < token.type.length(); ++i) {
                    if (token.type[i] == '\\' && i + 1 < token.type.length()) {
                        switch (token.type[i + 1]) {
                            case '"': decodedType += '"'; break;
                            case '\\': decodedType += '\\'; break;
                            case 'n': decodedType += '\n'; break;
                            case 't': decodedType += '\t'; break;
                            case 'r': decodedType += '\r'; break;
                            default: decodedType += token.type[i + 1]; break;
                        }
                        i++;
                    } else {
                        decodedType += token.type[i];
                    }
                }
                token.type = decodedType;
            }
        }
        
        size_t valueStart = tokenObj.find("\"value\":\"");
        if (valueStart != std::string::npos) {
            valueStart += 9;
            size_t valueEnd = tokenObj.find('"', valueStart);
            if (valueEnd != std::string::npos) {
                token.value = tokenObj.substr(valueStart, valueEnd - valueStart);
                
                std::string decodedValue;
                for (size_t i = 0; i < token.value.length(); ++i) {
                    if (token.value[i] == '\\' && i + 1 < token.value.length()) {
                        switch (token.value[i + 1]) {
                            case '"': decodedValue += '"'; break;
                            case '\\': decodedValue += '\\'; break;
                            case 'n': decodedValue += '\n'; break;
                            case 't': decodedValue += '\t'; break;
                            case 'r': decodedValue += '\r'; break;
                            default: decodedValue += token.value[i + 1]; break;
                        }
                        i++;
                    } else {
                        decodedValue += token.value[i];
                    }
                }
                token.value = decodedValue;
            }
        }
        
        size_t startStart = tokenObj.find("\"start\":");
        if (startStart != std::string::npos) {
            startStart += 8;
            size_t startEnd = tokenObj.find_first_of(",}", startStart);
            if (startEnd != std::string::npos) {
                std::string startStr = tokenObj.substr(startStart, startEnd - startStart);
                try {
                    token.start = std::stoul(startStr);
                } catch (...) {
                    token.start = 0;
                }
            }
        }
        
        if (!token.type.empty()) {
            parserTokens.push_back(token);
        }
        
        pos = objEnd + 1;
        
        if (pos < jsonStr.length() && jsonStr[pos] == ',') {
            pos++;
        } else if (pos < jsonStr.length() && jsonStr[pos] == ']') {
            break;
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
        
        for (char c : token.type) {
            if (c == '"') json << "\\\"";
            else if (c == '\\') json << "\\\\";
            else json << c;
        }
        
        json << "\",";
        json << "\"value\":\"";
        
        for (char c : token.value) {
            if (c == '"') json << "\\\"";
            else if (c == '\\') json << "\\\\";
            else if (c == '\n') json << "\\n";
            else if (c == '\t') json << "\\t";
            else if (c == '\r') json << "\\r";
            else json << c;
        }
        
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
            std::string inputStr(tokensJson);
            auto lexerResult = Lexer::parse(inputStr);
            
            for (const auto& token : lexerResult.second) {
                parserTokens.push_back({token.type, token.value, token.start});
            }
            
            std::string result = Parser::parseTokens(parserTokens);
            return strdup(result.c_str());
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
