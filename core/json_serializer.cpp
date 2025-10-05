#include "json_serializer.h"
#include <sstream>
#include <algorithm>

std::string JsonSerializer::escapeJsonString(const std::string& str) {
    std::stringstream ss;
    for (char c : str) {
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
                    // Escape control characters
                    ss << "\\u00" << std::hex << static_cast<int>(c);
                } else {
                    ss << c;
                }
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

std::string JsonSerializer::tokensToJson(const std::vector<Token>& tokens) {
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
    
    json << "}";
    return json.str();
}

std::string JsonSerializer::serialize(const std::vector<Token>& tokens, const std::string& input) {
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
        json << "\"position\":" << log.position;
        json << "}";
        
        if (i < logs.size() - 1) {
            json << ",";
        }
    }
    
    json << "]";
    return json.str();
}
