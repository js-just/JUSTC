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

#include "run.lua.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <cstring>
#include <utility>
#include <sstream>
#include <chrono>
#include <thread>
#include <atomic>

extern "C" {
    #include "luawrapper.h"
}

class LuaStateManager {
private:
    void* L;
    bool ownsState;

    std::string valueToString(int index) {
        int type = lua_wrapper_type(L, index);
        switch (type) {
            case 4: // LUA_TSTRING
                return lua_wrapper_tostring(L, index);
            case 3: // LUA_TNUMBER
                return std::to_string(lua_wrapper_tonumber(L, index));
            case 1: // LUA_TBOOLEAN
                return lua_wrapper_toboolean(L, index) ? "true" : "false";
            case 0: // LUA_TNIL
                return "null";
            default:
                return lua_wrapper_typename(L, type);
        }
    }

    void restrictEnvironment() {
        const char* dangerous_globals[] = {
            "dofile", "loadfile", "io", "os", "package", "debug",
            "collectgarbage", "getmetatable", "setmetatable",
            "load", "loadstring", "rawget", "rawset", "rawequal",
            nullptr
        };

        for (int i = 0; dangerous_globals[i] != nullptr; i++) {
            lua_wrapper_pushnil(L);
            lua_wrapper_setglobal(L, dangerous_globals[i]);
        }

        lua_wrapper_getglobal(L, "string");
        lua_wrapper_setglobal(L, "string");

        lua_wrapper_getglobal(L, "table");
        lua_wrapper_setglobal(L, "table");

        lua_wrapper_getglobal(L, "math");
        lua_wrapper_setglobal(L, "math");

        lua_wrapper_getglobal(L, "utf8");
        lua_wrapper_setglobal(L, "utf8");

        lua_wrapper_getglobal(L, "coroutine");
        lua_wrapper_setglobal(L, "coroutine");
    }

public:
    LuaStateManager() : L(nullptr), ownsState(true) {
        L = lua_wrapper_newstate();
        if (!L) {
            throw std::runtime_error("Failed to create Lua state (Lua).");
        }
        lua_wrapper_openlibs(L);
        restrictEnvironment();
    }

    ~LuaStateManager() {
        if (ownsState && L) {
            lua_wrapper_close(L);
        }
    }

    void* getState() { return L; }

    void execute(const std::string& code, int numReturns = 0) {
        if (code.empty()) return;

        int result = lua_wrapper_loadstring(L, code.c_str());
        if (result != 0) {
            std::string error = lua_wrapper_tostring(L, -1);
            lua_wrapper_pop(L, 1);
            throw std::runtime_error("Lua compilation error: " + error);
        }

        result = lua_wrapper_pcall(L, 0, numReturns, 0);
        if (result != 0) {
            std::string error = lua_wrapper_tostring(L, -1);
            lua_wrapper_pop(L, 1);
            throw std::runtime_error("Lua runtime error: " + error);
        }
    }

    std::string getTopValueAsString() {
        return valueToString(-1);
    }

    int getTopValueType() {
        return lua_wrapper_type(L, -1);
    }

    void pop(int n = 1) {
        lua_wrapper_pop(L, n);
    }
};

static std::string luaTableToJSON(void* L, int index) {
    std::string result = "{";
    bool first = true;

    lua_wrapper_pushnil(L);
    while (lua_wrapper_next(L, index) != 0) {
        if (!first) result += ",";
        first = false;

        int keyType = lua_wrapper_type(L, -2);
        if (keyType == 4) { // LUA_TSTRING
            const char* key = lua_wrapper_tostring(L, -2);
            result += "\"" + std::string(key) + "\":";
        } else if (keyType == 3) { // LUA_TNUMBER
            double keyNum = lua_wrapper_tonumber(L, -2);
            result += std::to_string(keyNum) + ":";
        } else {
            result += "\"key\":";
        }

        int valType = lua_wrapper_type(L, -1);
        switch (valType) {
            case 4: { // LUA_TSTRING
                const char* val = lua_wrapper_tostring(L, -1);
                std::string escaped;
                for (const char* p = val; *p; p++) {
                    if (*p == '"') escaped += "\\\"";
                    else if (*p == '\\') escaped += "\\\\";
                    else if (*p == '\n') escaped += "\\n";
                    else if (*p == '\r') escaped += "\\r";
                    else if (*p == '\t') escaped += "\\t";
                    else escaped += *p;
                }
                result += "\"" + escaped + "\"";
                break;
            }
            case 3: // LUA_TNUMBER
                result += std::to_string(lua_wrapper_tonumber(L, -1));
                break;
            case 1: // LUA_TBOOLEAN
                result += lua_wrapper_toboolean(L, -1) ? "true" : "false";
                break;
            case 0: // LUA_TNIL
                result += "null";
                break;
            case 5: // LUA_TTABLE
                result += luaTableToJSON(L, lua_wrapper_gettop(L));
                break;
            default:
                result += "\"" + std::string(lua_wrapper_typename(L, valType)) + "\"";
                break;
        }

        lua_wrapper_pop(L, 1);
    }

    result += "}";
    return result;
}

static std::string luaArrayToJSON(void* L, int index) {
    std::string result = "[";
    bool first = true;

    int len = lua_wrapper_objlen(L, index);

    for (int i = 1; i <= len; i++) {
        if (!first) result += ",";
        first = false;

        lua_wrapper_rawgeti(L, index, i);

        int valType = lua_wrapper_type(L, -1);
        switch (valType) {
            case 4: { // LUA_TSTRING
                const char* val = lua_wrapper_tostring(L, -1);
                std::string escaped;
                for (const char* p = val; *p; p++) {
                    if (*p == '"') escaped += "\\\"";
                    else if (*p == '\\') escaped += "\\\\";
                    else if (*p == '\n') escaped += "\\n";
                    else if (*p == '\r') escaped += "\\r";
                    else if (*p == '\t') escaped += "\\t";
                    else escaped += *p;
                }
                result += "\"" + escaped + "\"";
                break;
            }
            case 3: // LUA_TNUMBER
                result += std::to_string(lua_wrapper_tonumber(L, -1));
                break;
            case 1: // LUA_TBOOLEAN
                result += lua_wrapper_toboolean(L, -1) ? "true" : "false";
                break;
            case 0: // LUA_TNIL
                result += "null";
                break;
            case 5: // LUA_TTABLE
                if (lua_wrapper_objlen(L, -1) > 0) {
                    result += luaArrayToJSON(L, lua_wrapper_gettop(L));
                } else {
                    result += luaTableToJSON(L, lua_wrapper_gettop(L));
                }
                break;
            default:
                result += "\"" + std::string(lua_wrapper_typename(L, valType)) + "\"";
                break;
        }

        lua_wrapper_pop(L, 1);
    }

    result += "]";
    return result;
}

void RunLua::runScript(const std::string& code) {
    LuaStateManager luaManager;
    luaManager.execute(code);
}

std::pair<std::string, int> RunLua::runScriptWithResult(const std::string& code) {
    LuaResult result = runScriptWithDetailedResult(code);
    return {result.value, result.type};
}

LuaResult RunLua::runScriptWithDetailedResult(const std::string& code) {
    LuaStateManager luaManager;
    void* L = luaManager.getState();

    int result = lua_wrapper_loadstring(L, code.c_str());
    if (result != 0) {
        const char* error = lua_wrapper_tostring(L, -1);
        std::string errMsg = error ? error : "Unknown Lua compilation error";
        lua_wrapper_pop(L, 1);
        return LuaResult(errMsg);
    }

    result = lua_wrapper_pcall(L, 0, 1, 0);
    if (result != 0) {
        const char* error = lua_wrapper_tostring(L, -1);
        std::string errMsg = error ? error : "Unknown Lua runtime error";
        lua_wrapper_pop(L, 1);
        return LuaResult(errMsg);
    }

    std::string output;
    int outputType = 0;
    int type = lua_wrapper_type(L, -1);

    switch (type) {
        case 3: // LUA_TNUMBER
            output = std::to_string(lua_wrapper_tonumber(L, -1));
            outputType = 1;
            break;
        case 1: // LUA_TBOOLEAN
            output = lua_wrapper_toboolean(L, -1) ? "true" : "false";
            outputType = 2;
            break;
        case 0: // LUA_TNIL
            output = "null";
            outputType = 3;
            break;
        case 4: // LUA_TSTRING
            output = lua_wrapper_tostring(L, -1);
            outputType = 0;
            break;
        case 5: { // LUA_TTABLE
            int len = lua_wrapper_objlen(L, -1);
            if (len > 0) {
                output = luaArrayToJSON(L, -1);
                outputType = 5;
            } else {
                output = luaTableToJSON(L, -1);
                outputType = 4;
            }
            break;
        }
        case 6: // LUA_TFUNCTION
            output = "[Lua Function]";
            outputType = 6;
            break;
        case 7: // LUA_TTHREAD
            output = "[Lua Thread]";
            outputType = 7;
            break;
        case 8: case 9: // LUA_TLIGHTUSERDATA, LUA_TUSERDATA
            output = "[Lua UserData]";
            outputType = 8;
            break;
        default:
            output = "[Lua " + std::string(lua_wrapper_typename(L, type)) + "]";
            outputType = 0;
            break;
    }

    lua_wrapper_pop(L, 1);
    return LuaResult(output, outputType);
}

bool RunLua::compileScript(const std::string& code, std::string& error) {
    LuaStateManager luaManager;
    void* L = luaManager.getState();

    int result = lua_wrapper_loadstring(L, code.c_str());
    if (result != 0) {
        const char* err = lua_wrapper_tostring(L, -1);
        error = err ? err : "Compilation failed";
        lua_wrapper_pop(L, 1);
        return false;
    }

    lua_wrapper_pop(L, 1);
    return true;
}

LuaResult RunLua::runScriptWithGlobals(const std::string& code, const std::unordered_map<std::string, std::string>& globals) {
    LuaStateManager luaManager;
    lua_State* L = luaManager.getState();

    for (const auto& [key, value] : globals) {
        lua_wrapper_pushstring(L, value.c_str());
        lua_wrapper_setglobal(L, key.c_str());
    }

    int result = lua_wrapper_loadstring(L, code.c_str());
    if (result != LUA_OK) {
        const char* error = lua_wrapper_tostring(L, -1);
        std::string errMsg = error ? error : "Unknown Lua compilation error";
        lua_wrapper_pop(L, 1);
        return LuaResult(errMsg);
    }

    result = lua_wrapper_pcall(L, 0, 1, 0);
    if (result != LUA_OK) {
        const char* error = lua_wrapper_tostring(L, -1);
        std::string errMsg = error ? error : "Unknown Lua runtime error";
        lua_wrapper_pop(L, 1);
        return LuaResult(errMsg);
    }

    std::string output;
    int outputType = 0;
    int type = lua_wrapper_type(L, -1);

    switch (type) {
        case LUA_TNUMBER:
            output = std::to_string(lua_wrapper_tonumber(L, -1));
            outputType = 1;
            break;
        case LUA_TBOOLEAN:
            output = lua_wrapper_toboolean(L, -1) ? "true" : "false";
            outputType = 2;
            break;
        case LUA_TNIL:
            output = "null";
            outputType = 3;
            break;
        case LUA_TSTRING:
            output = lua_wrapper_tostring(L, -1);
            outputType = 0;
            break;
        default:
            output = lua_wrapper_typename(L, type);
            outputType = 0;
            break;
    }

    lua_wrapper_pop(L, 1);
    return LuaResult(output, outputType);
}

LuaResult RunLua::runScriptWithTimeout(const std::string& code, int timeoutMs) {
    std::atomic<bool> completed(false);
    LuaResult result;
    std::exception_ptr exceptionPtr = nullptr;

    std::thread worker([&]() {
        try {
            result = runScriptWithDetailedResult(code);
            completed = true;
        } catch (...) {
            exceptionPtr = std::current_exception();
            completed = true;
        }
    });

    auto start = std::chrono::steady_clock::now();
    while (!completed) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > timeoutMs) {
            worker.detach();
            return LuaResult("Script execution timeout (" + std::to_string(timeoutMs) + "ms)");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    worker.join();

    if (exceptionPtr) {
        try {
            std::rethrow_exception(exceptionPtr);
        } catch (const std::exception& e) {
            return LuaResult(e.what());
        }
    }

    return result;
}

#ifdef __EMSCRIPTEN__

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    char* lua_execute(const char* code) {
        try {
            auto result = RunLua::runScriptWithResult(std::string(code));
            char* output = (char*)malloc(result.first.length() + 1);
            strcpy(output, result.first.c_str());
            return output;
        } catch (const std::exception& e) {
            char* output = (char*)malloc(strlen(e.what()) + 1);
            strcpy(output, e.what());
            return output;
        }
    }

    EMSCRIPTEN_KEEPALIVE
    void lua_execute_async(const char* code, void (*callback)(const char*, int)) {
        try {
            auto result = RunLua::runScriptWithResult(std::string(code));
            callback(result.first.c_str(), result.second);
        } catch (const std::exception& e) {
            callback(e.what(), -1);
        }
    }
}

void RunLua::runScriptAsync(const std::string& code, void (*callback)(const LuaResult&)) {
    std::thread([code, callback]() {
        auto result = runScriptWithDetailedResult(code);
        callback(result);
    }).detach();
}
#endif
