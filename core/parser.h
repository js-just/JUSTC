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

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

enum class DataType {
    JUSTC_OBJECT = 0,
    NUMBER = 1,
    STRING = 2,
    LINK = 3,
    BOOLEAN = 4,
    JSON_OBJECT = 5,
    JSON_ARRAY = 6,
    NULL_TYPE = 7,
    HEXADECIMAL = 9,
    BINARY = 11,
    PATH = 12,
    ERROR = 13,
    VARIABLE = 14,
    FUNCTION = 15,
    NOT_A_NUMBER = 17,
    INFINITE = 18,
    SYNTAX_ERROR = 19,
    OCTAL = 20,
    UNKNOWN = -1
};

struct Value {
    DataType type;
    
    union {
        double number_value;
        bool boolean_value;
    };
    std::string string_value;
    std::shared_ptr<void> complex_value;
    
    Value() : type(DataType::UNKNOWN), number_value(0) {}
    Value(DataType t) : type(t), number_value(0) {}
    
    std::string toString() const;
    double toNumber() const;
    bool toBoolean() const;
    
    static Value createNumber(double num);
    static Value createString(const std::string& str);
    static Value createBoolean(bool b);
    static Value createNull();
    static Value createLink(const std::string& link);
    static Value createPath(const std::string& path);
    static Value createVariable(const std::string& varName);
};

struct LogEntry {
    std::string type;
    std::string message;
    size_t position;
    std::string timestamp;
    
    LogEntry(const std::string& t, const std::string& m, size_t p, const std::string& ts = "") 
        : type(t), message(m), position(p), timestamp(ts) {}
};

struct ParseResult {
    std::unordered_map<std::string, Value> returnValues;
    std::vector<LogEntry> logs;
    std::string logFilePath;
    std::string logFileContent;
    std::string error;
    
    ParseResult() : logFilePath(""), logFileContent(""), error("") {}
};

struct JSONObject {
    std::unordered_map<std::string, Value> properties;
};

struct JSONArray {
    std::vector<Value> elements;
};

struct JUSTCObject {
    std::unordered_map<std::string, Value> variables;
    std::vector<std::string> outputOrder;
};

struct ASTNode {
    std::string type;
    std::string identifier;
    Value value;
    std::vector<std::string> references;
    std::vector<std::string> tokens;
    size_t startPos;
    
    ASTNode(const std::string& t, const std::string& id = "", size_t start = 0) 
        : type(t), identifier(id), startPos(start) {}
};

struct ParserToken {
    std::string type;
    std::string value;
    size_t start;

    ParserToken() : type(""), value(""), start(0) {}
    ParserToken(const std::string& t, const std::string& v, size_t s) 
        : type(t), value(v), start(s) {}
};

class Parser {
private:
    std::vector<ParserToken> tokens;
    std::vector<ASTNode> ast;
    size_t position;
    
    std::unordered_map<std::string, Value> variables;
    std::unordered_map<std::string, std::vector<std::string>> dependencies;
    std::vector<std::string> outputVariables;
    std::vector<std::string> outputNames;
    std::string outputMode;
    bool allowJavaScript;
    bool globalScope;
    bool strictMode;
    
    std::vector<LogEntry> logs;
    std::string logFilePath;
    std::string logFileContent;
    bool hasLogFile;
    
    ParserToken currentToken() const;
    ParserToken peekToken(size_t offset = 1) const;
    void advance();
    bool match(const std::string& type) const;
    bool match(const std::string& type, const std::string& value) const;
    bool isEnd() const;
    void skipCommas();

    std::string getCurrentTimestamp() const;
    
    // logs
    void addLog(const std::string& type, const std::string& message, size_t position = 0);
    void setLogFile(const std::string& path);
    void appendToLogFile(const std::string& content);
    
    Value parseExpression();
    Value parsePrimary();
    Value parseConditional();
    Value parseLogicalOR();
    Value parseLogicalAND();
    Value parseEquality();
    Value parseComparison();
    Value parseTerm();
    Value parseFactor();
    Value parsePower();
    Value parseUnary();
    Value parseFunctionCall();
    
    ASTNode parseStatement();
    ASTNode parseVariableDeclaration();
    ASTNode parseCommand();
    ASTNode parseTypeCommand();
    ASTNode parseOutputCommand();
    ASTNode parseReturnCommand();
    ASTNode parseAllowCommand();
    ASTNode parseImportCommand();
    
    Value executeFunction(const std::string& funcName, const std::vector<Value>& args);
    Value evaluateExpression(const Value& left, const std::string& op, const Value& right);
    Value handleInequality(const Value& value);
    Value handleConditional(const Value& condition, const Value& thenVal, const Value& elseVal, 
                           const std::string& thenOp, const std::string& elseOp);
    
    void buildDependencyGraph();
    bool detectCycles();
    bool dfsCycleDetection(const std::string& node, 
                          std::unordered_map<std::string, bool>& visited,
                          std::unordered_map<std::string, bool>& recStack,
                          std::vector<std::string>& cyclePath);
    
    Value resolveVariableValue(const std::string& varName);
    void evaluateAllVariables();
    Value evaluateASTNode(const ASTNode& node);
    void extractReferences(const Value& value, std::vector<std::string>& references);
    
    Value stringToValue(const std::string& str);
    Value numberToValue(double num);
    Value booleanToValue(bool b);
    Value linkToValue(const std::string& link);
    Value pathToValue(const std::string& path);
    Value hexToValue(const std::string& hexStr);
    Value binaryToValue(const std::string& binStr);
    Value octalToValue(const std::string& octStr);
    
    // built-in
    Value functionVALUE(const std::vector<Value>& args);
    Value functionSTRING(const std::vector<Value>& args);
    Value functionLINK(const std::vector<Value>& args);
    Value functionNUMBER(const std::vector<Value>& args);
    Value functionBINARY(const std::vector<Value>& args);
    Value functionOCTAL(const std::vector<Value>& args);
    Value functionHEXADECIMAL(const std::vector<Value>& args);
    Value functionTYPEID(const std::vector<Value>& args);
    Value functionTYPEOF(const std::vector<Value>& args);
    Value functionECHO(const std::vector<Value>& args);
    Value functionJSON(const std::vector<Value>& args);
    Value functionHTTPJSON(const std::vector<Value>& args);
    Value functionHTTPTEXT(const std::vector<Value>& args);
    Value functionJUSTC(const std::vector<Value>& args);
    Value functionHTTPJUSTC(const std::vector<Value>& args);
    Value functionPARSEJUSTC(const std::vector<Value>& args);
    Value functionPARSEJSON(const std::vector<Value>& args);
    Value functionFILE(const std::vector<Value>& args);
    Value functionSTAT(const std::vector<Value>& args);
    Value functionENV(const std::vector<Value>& args);
    Value functionCONFIG(const std::vector<Value>& args);
    
    // math
    Value functionV(const std::vector<Value>& args);
    Value functionD(const std::vector<Value>& args);
    Value functionSQ(const std::vector<Value>& args);
    Value functionCU(const std::vector<Value>& args);
    Value functionP(const std::vector<Value>& args);
    Value functionM(const std::vector<Value>& args);
    Value functionS(const std::vector<Value>& args);
    Value functionC(const std::vector<Value>& args);
    Value functionT(const std::vector<Value>& args);
    Value functionN(const std::vector<Value>& args);
    Value functionABSOLUTE(const std::vector<Value>& args);
    Value functionCEIL(const std::vector<Value>& args);
    Value functionFLOOR(const std::vector<Value>& args);
    
public:
    Parser(const std::vector<ParserToken>& tokens);
    
    ParseResult parse();
    
    static ParseResult parseTokens(const std::vector<ParserToken>& tokens);
};

#endif // PARSER_H
