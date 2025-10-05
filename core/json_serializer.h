#ifndef JSON_SERIALIZER_H
#define JSON_SERIALIZER_H

#include "parser.h"
#include <string>

class JsonSerializer {
public:
    static std::string serialize(const ParseResult& result);
    static std::string serialize(const std::vector<ParserToken>& tokens, const std::string& input);
    static std::string serialize(const std::vector<LogEntry>& logs);
    
private:
    static std::string escapeJsonString(const std::string& str);
    static std::string valueToJson(const Value& value);
    static std::string tokensToJson(const std::vector<ParserToken>& tokens);
};

#endif // JSON_SERIALIZER_H
