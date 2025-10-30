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

#include "json.h"
#include <sstream>
#include <algorithm>

std::string JsonSerializer::escapeJsonString(const std::string& str) {
    std::stringstream ss;
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        switch (c) {
            case '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: 
                if (static_cast<unsigned char>(c) < 0x20 || static_cast<unsigned char>(c) == 0x7F) {
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                    ss << buf;
                } else {
                    ss << c;
                }
                break;
        }
        
        if (i > 100000) {
            ss << "...(truncated)";
            break;
        }
    }
    return ss.str();
}

std::string JsonSerializer::valueToJson(const Value& value) {
    switch (value.type) {
        case DataType::NUMBER:
        case DataType::HEXADECIMAL:
        case DataType::BINARY:
        case DataType::OCTAL:
            return std::to_string(value.number_value);
        case DataType::STRING:
        case DataType::LINK:
        case DataType::PATH:
        case DataType::VARIABLE:
            return "\"" + escapeJsonString(value.string_value) + "\"";
        case DataType::BOOLEAN:
            return value.boolean_value ? "true" : "false";
        case DataType::NULL_TYPE:
            return "null";
        case DataType::NOT_A_NUMBER:
            return "\"NaN\"";
        case DataType::INFINITE:
            return "\"Infinity\"";
        default:
            return "\"unknown\"";
    }
}

std::string JsonSerializer::tokensToJson(const std::vector<ParserToken>& tokens) {
    std::stringstream json;
    json << "[";
    
    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& token = tokens[i];
        
        json << "{";
        json << "\"type\":\"" << escapeJsonString(token.type) << "\",";
        json << "\"value\":\"" << escapeJsonString(token.value) << "\",";
        json << "\"start\":" << token.start;
        json << "}";
        
        if (i < tokens.size() - 1) {
            json << ",";
        }
    }
    
    json << "]";
    return json.str();
}

std::string JsonSerializer::serialize(const ParseResult& result) {
    std::stringstream json;
    json << "{";
    
    #ifdef __EMSCRIPTEN__
    
    if (!result.error.empty()) {
        json << "\"error\":\"" << escapeJsonString(result.error) << "\"";
    } else {
        // return values
        json << "\"return\":{";
        bool first = true;
        for (const auto& pair : result.returnValues) {
            if (!first) json << ",";
            first = false;
            json << "\"" << escapeJsonString(pair.first) << "\":" << valueToJson(pair.second);
        }
        json << "},";
        
        // logs array
        json << "\"logs\":";
        json << serialize(result.logs);
        json << ",";
        
        // logfile object
        json << "\"logfile\":{";
        json << "\"file\":\"" << escapeJsonString(result.logFilePath) << "\",";
        json << "\"logs\":\"" << escapeJsonString(result.logFileContent) << "\"";
        json << "}";
    }

    #else

    bool first = true;
    for (const auto& pair : result.returnValues) {
        if (!first) json << ",";
        first = false;
        json << "\"" << escapeJsonString(pair.first) << "\":" << valueToJson(pair.second);
    }

    #endif
    
    json << "}";
    return json.str();
}

std::string JsonSerializer::serialize(const std::vector<ParserToken>& tokens, const std::string& input) {
    std::stringstream json;
    json << "{";
    json << "\"input\":\"" << escapeJsonString(input) << "\",";
    json << "\"tokens\":" << tokensToJson(tokens);
    json << "}";
    return json.str();
}

std::string JsonSerializer::serialize(const std::vector<LogEntry>& logs) {
    std::stringstream json;
    json << "[";
    
    for (size_t i = 0; i < logs.size(); i++) {
        const auto& log = logs[i];
        json << "{";
        json << "\"type\":\"" << escapeJsonString(log.type) << "\",";
        json << "\"message\":\"" << escapeJsonString(log.message) << "\",";
        json << "\"position\":" << log.position << ",";
        json << "\"time\":\"" << escapeJsonString(log.timestamp) << "\"";
        json << "}";
        
        if (i < logs.size() - 1) {
            json << ",";
        }
    }
    
    json << "]";
    return json.str();
}
