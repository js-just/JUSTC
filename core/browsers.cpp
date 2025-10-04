#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include "lexer.h"

extern "C" {

char* lexer(const char* input) {
    if (input == nullptr) return nullptr;
    
    try {
        auto parsed = Lexer::parse(input);
        auto& tokens = parsed.second;
        
        std::stringstream json;
        json << "{";
        json << "\"input\":\"";
        
        for (char c : parsed.first) {
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