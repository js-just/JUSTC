#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"
#include <string>

class Interpreter {
public:
    static ParseResult interpret(const std::string& code);
    
private:
    static Value fetchHttpContent(const std::string& url, const std::string& expectedType);
    static std::string executeHttpRequest(const std::string& url);
    void processHttpRequests(const ParseResult& result);
};

#endif // INTERPRETER_H
