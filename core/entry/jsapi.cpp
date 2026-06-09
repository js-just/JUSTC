/*

MIT License

Copyright (c) 2025-2026 JustStudio. <https://juststudio.is-a.dev/>

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

#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include "../lexer.h"
#include "../parser.h"
#include "../json.hpp"
#include "../fetch.h"
#include "../version.h"
#include <tuple>
#include "../justo.hpp"
#include <functional>
#include <mutex>
#include <unordered_map>
#include <memory>
#include <algorithm>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

template<typename... Args>
std::string outputString(std::string mode, Args... args) {
    if (mode == "xml") {
        return XmlSerializer::serialize(args...);
    } else if (mode == "yaml") {
        return YamlSerializer::serialize(args...);
    } else {
        return JsonSerializer::serialize(args...);
    }
}

static std::unique_ptr<Parser> globalParser = nullptr;
static std::mutex globalParserMutex;

static std::unordered_map<std::string, Value> justoPointers;
static std::mutex justoPointersMutex;

static std::vector<std::function<void(const std::string&, const Value&)>> varUpdateListeners;
static std::mutex varUpdateListenersMutex;

void ensureGlobalParser() {
    if (!globalParser) {
        globalParser = std::make_unique<Parser>(
            std::vector<ParserToken>(), true, false, "", true, true,
            "global", "global", true, true, false, nullptr, CharType::GRAPHEME
        );
    }
}

void initializeJUSTOPointers() {
    std::lock_guard<std::mutex> lock(justoPointersMutex);

    Value nanVal;
    nanVal.type = DataType::NOT_A_NUMBER;
    nanVal.name = "NaN";
    justoPointers["nan"] = nanVal;

    Value infVal;
    infVal.type = DataType::INFINITE;
    infVal.name = "Infinity";
    justoPointers["inf"] = infVal;
}

Value justoToValue(const std::string& justo) {
    JUSTO::JUSTOParser parser;
    for (const auto& [key, value] : justoPointers) {
        parser.registerPointer(key, value);
    }
    return parser.parse(code);
}

void triggerVariableUpdate(const std::string& name, const Value& value) {
    std::lock_guard<std::mutex> lock(varUpdateListenersMutex);
    for (const auto& listener : varUpdateListeners) {
        try {
            listener(name, value);
        } catch (const std::exception& e) {}
    }
}

#ifdef __EMSCRIPTEN__
extern "C" {
    void jsVariableUpdateCallback(const char* name, const char* valueJUSTO) {
        if (name == nullptr || valueJUSTO == nullptr) return;

        Value val = justoToValue(std::string(valueJUSTO));
        triggerVariableUpdate(std::string(name), val);
    }
}
#endif

extern "C" {

char* lexer(const char* input, const char* outputMode) {
    if (input == nullptr) return nullptr;
    std::string mode(outputMode == nullptr ? "json" : outputMode);

    try {
        auto parsed = Lexer::parse(input, true);
        std::string json = outputString(mode, parsed.second, parsed.first);
        return strdup(json.c_str());

    } catch (const std::exception& e) {
        std::string error = "{\"error\":\"" + JsonSerializer::escapeJsonString(std::string(e.what())) + "\",\"lexer\":true}";
        return strdup(error.c_str());
    }
}

char* parser(const char* tokensJson, const char* outputMode) {
    if (tokensJson == nullptr) return nullptr;
    std::string mode(outputMode == nullptr ? "json" : outputMode);

    try {
        std::vector<ParserToken> parserTokens;
        std::string input = "";

        if (JsonParser::parseJsonTokens(tokensJson, parserTokens, input)) {
            ParseResult result = Parser::parseTokens(parserTokens, false, false, input);
            std::string json = outputString(mode, result);
            return strdup(json.c_str());
        } else {
            std::string error = "{\"error\":\"Failed to parse tokens JSON\"}";
            return strdup(error.c_str());
        }

    } catch (const std::exception& e) {
        std::string error = "{\"error\":\"" + JsonSerializer::escapeJsonString(std::string(e.what())) + "\",\"parser\":true}";
        return strdup(error.c_str());
    }
}

char* parse(const char* input, const bool execute, const bool runAsync, const char* outputMode) {
    if (input == nullptr) return nullptr;
    std::string mode(outputMode == nullptr ? "json" : outputMode);

    try {
        auto lexerResult = Lexer::parse(input);
        ParseResult result = Parser::parseTokens(lexerResult.second, execute, runAsync, input);

        if (result.variables) {
            for (const auto& [key, value] : *result.variables) {
                if (globalParser && globalParser->hasGlobal(key)) {
                    Value oldVal = globalParser->getGlobal(key);
                    if (oldVal.toString() != value.toString()) {
                        globalParser->registerGlobal(key, value);
                        triggerVariableUpdate(key, value);
                    }
                }
            }
        }

        std::string json = outputString(mode, result);
        return strdup(json.c_str());

    } catch (const std::exception& e) {
        std::string error = "{\"error\":\"" + JsonSerializer::escapeJsonString(std::string(e.what())) + "\"}";
        return strdup(error.c_str());
    }
}

void free_string(char* str) {
    if (str != nullptr) {
        free(str);
    }
}

char* version() {
    return strdup(JUSTC_VERSION.c_str());
}

int registerGlobal(const char* name, const char* justoValue) {
    if (name == nullptr || justoValue == nullptr) return 0;

    std::lock_guard<std::mutex> lock(globalParserMutex);
    ensureGlobalParser();

    try {
        Value val = justoToValue(std::string(justoValue));
        globalParser->registerGlobal(std::string(name), val);
        triggerVariableUpdate(std::string(name), val);
        return 1;
    } catch (const std::exception& e) {
        return 0;
    }
}

char* getGlobal(const char* name) {
    if (name == nullptr) return strdup(";");

    std::lock_guard<std::mutex> lock(globalParserMutex);
    ensureGlobalParser();

    try {
        Value val = globalParser->getGlobal(std::string(name));
        std::string justo = valueToJUSTO(val);
        return strdup(justo.c_str());
    } catch (const std::exception& e) {
        return strdup(";");
    }
}

int hasGlobal(const char* name) {
    if (name == nullptr) return 0;

    std::lock_guard<std::mutex> lock(globalParserMutex);
    ensureGlobalParser();

    return globalParser->hasGlobal(std::string(name)) ? 1 : 0;
}

int unregisterGlobal(const char* name) {
    if (name == nullptr) return 0;

    std::lock_guard<std::mutex> lock(globalParserMutex);
    ensureGlobalParser();

    globalParser->unregisterGlobal(std::string(name));
    triggerVariableUpdate(std::string(name), Value::createNull());
    return 1;
}

void clearGlobals() {
    std::lock_guard<std::mutex> lock(globalParserMutex);
    ensureGlobalParser();
    globalParser->clearGlobals();
}

int registerPointer(const char* name, const char* justoValue) {
    if (name == nullptr || justoValue == nullptr) return 0;

    std::lock_guard<std::mutex> lock(justoPointersMutex);

    try {
        JUSTO::JustoParser parser(justoPointers);
        Value val = parser.parse(std::string(justoValue));
        justoPointers[std::string(name)] = val;
        return 1;
    } catch (const std::exception& e) {
        return 0;
    }
}

char* getPointer(const char* name) {
    if (name == nullptr) return strdup(";");

    std::lock_guard<std::mutex> lock(justoPointersMutex);

    auto it = justoPointers.find(std::string(name));
    if (it != justoPointers.end()) {
        std::string justo = valueToJUSTO(it->second);
        return strdup(justo.c_str());
    }
    return strdup(";");
}

int unregisterPointer(const char* name) {
    if (name == nullptr) return 0;

    std::lock_guard<std::mutex> lock(justoPointersMutex);
    justoPointers.erase(std::string(name));
    return 1;
}

void clearPointers() {
    std::lock_guard<std::mutex> lock(justoPointersMutex);
    justoPointers.clear();
    initializeJUSTOPointers();
}

int addVariableUpdateListener(void (*callback)(const char* name, const char* valueJUSTO)) {
    if (callback == nullptr) return 0;

    std::lock_guard<std::mutex> lock(varUpdateListenersMutex);

    varUpdateListeners.push_back([callback](const std::string& name, const Value& value) {
        std::string justo = valueToJUSTO(value);
        callback(name.c_str(), justo.c_str());
    });

    return 1;
}

void clearVariableUpdateListeners() {
    std::lock_guard<std::mutex> lock(varUpdateListenersMutex);
    varUpdateListeners.clear();
}

}

struct JUSTOInitializer {
    JUSTOInitializer() {
        initializeJUSTOPointers();
    }
} justoInitializer;
