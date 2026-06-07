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

#pragma push_macro("LUA_H")
#pragma push_macro("LUALIB_H")
#pragma push_macro("LAUXLIB_H")
#undef LUA_H
#undef LUALIB_H
#undef LAUXLIB_H

#define lua_State lua_State_orig
#define lua_newstate lua_newstate_orig
#define lua_close lua_close_orig
#define luaL_newstate luaL_newstate_orig
#define luaL_openlibs luaL_openlibs_orig
#define luaL_loadstring luaL_loadstring_orig
#define lua_pcall lua_pcall_orig
#define lua_tostring lua_tostring_orig
#define lua_pop lua_pop_orig
#define lua_type lua_type_orig
#define lua_typename lua_typename_orig
#define lua_tonumber lua_tonumber_orig
#define lua_toboolean lua_toboolean_orig
#define lua_touserdata lua_touserdata_orig
#define lua_getglobal lua_getglobal_orig
#define lua_setglobal lua_setglobal_orig
#define lua_pushnil lua_pushnil_orig
#define lua_pushnumber lua_pushnumber_orig
#define lua_pushstring lua_pushstring_orig
#define lua_next lua_next_orig
#define lua_getfield lua_getfield_orig
#define lua_getmetatable lua_getmetatable_orig
#define lua_objlen lua_objlen_orig
#define lua_gettop lua_gettop_orig
#define lua_settop lua_settop_orig
#define lua_pushvalue lua_pushvalue_orig
#define lua_insert lua_insert_orig
#define lua_remove lua_remove_orig
#define lua_replace lua_replace_orig
#define lua_createtable lua_createtable_orig
#define lua_setfield lua_setfield_orig
#define lua_rawgeti lua_rawgeti_orig
#define lua_rawseti lua_rawseti_orig
#define lua_gettable lua_gettable_orig
#define lua_settable lua_settable_orig

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#undef lua_State
#undef lua_newstate
#undef lua_close
#undef luaL_newstate
#undef luaL_openlibs
#undef luaL_loadstring
#undef lua_pcall
#undef lua_tostring
#undef lua_pop
#undef lua_type
#undef lua_typename
#undef lua_tonumber
#undef lua_toboolean
#undef lua_touserdata
#undef lua_getglobal
#undef lua_setglobal
#undef lua_pushnil
#undef lua_pushnumber
#undef lua_pushstring
#undef lua_next
#undef lua_getfield
#undef lua_getmetatable
#undef lua_objlen
#undef lua_gettop
#undef lua_settop
#undef lua_pushvalue
#undef lua_insert
#undef lua_remove
#undef lua_replace
#undef lua_createtable
#undef lua_setfield
#undef lua_rawgeti
#undef lua_rawseti
#undef lua_gettable
#undef lua_settable

using lua_State = struct lua_State_orig;
using lua_CFunction = int (*)(lua_State_orig*);

extern "C" {
    lua_State_orig* luaL_newstate_orig(void);
    void luaL_openlibs_orig(lua_State_orig* L);
    int luaL_loadstring_orig(lua_State_orig* L, const char* s);
    int lua_pcall_orig(lua_State_orig* L, int nargs, int nresults, int errfunc);
    const char* lua_tostring_orig(lua_State_orig* L, int idx);
    void lua_pop_orig(lua_State_orig* L, int n);
    int lua_type_orig(lua_State_orig* L, int idx);
    const char* lua_typename_orig(lua_State_orig* L, int tp);
    lua_Number lua_tonumber_orig(lua_State_orig* L, int idx);
    int lua_toboolean_orig(lua_State_orig* L, int idx);
    void* lua_touserdata_orig(lua_State_orig* L, int idx);
    void lua_getglobal_orig(lua_State_orig* L, const char* name);
    void lua_setglobal_orig(lua_State_orig* L, const char* name);
    void lua_pushnil_orig(lua_State_orig* L);
    void lua_pushnumber_orig(lua_State_orig* L, lua_Number n);
    void lua_pushstring_orig(lua_State_orig* L, const char* s);
    int lua_next_orig(lua_State_orig* L, int idx);
    void lua_getfield_orig(lua_State_orig* L, int idx, const char* k);
    int lua_getmetatable_orig(lua_State_orig* L, int idx);
    size_t lua_objlen_orig(lua_State_orig* L, int idx);
    int lua_gettop_orig(lua_State_orig* L);
    void lua_settop_orig(lua_State_orig* L, int idx);
    void lua_pushvalue_orig(lua_State_orig* L, int idx);
    void lua_insert_orig(lua_State_orig* L, int idx);
    void lua_remove_orig(lua_State_orig* L, int idx);
    void lua_replace_orig(lua_State_orig* L, int idx);
    void lua_createtable_orig(lua_State_orig* L, int narr, int nrec);
    void lua_setfield_orig(lua_State_orig* L, int idx, const char* k);
    void lua_rawgeti_orig(lua_State_orig* L, int idx, int n);
    void lua_rawseti_orig(lua_State_orig* L, int idx, int n);
    void lua_gettable_orig(lua_State_orig* L, int idx);
    void lua_settable_orig(lua_State_orig* L, int idx);
    void lua_close_orig(lua_State_orig* L);
    int luaL_loadstringx_orig(lua_State_orig* L, const char* s, const char* mode);
}

inline lua_State* luaL_newstate() { return (lua_State*)luaL_newstate_orig(); }
inline void luaL_openlibs(lua_State* L) { luaL_openlibs_orig((lua_State_orig*)L); }
inline int luaL_loadstring(lua_State* L, const char* s) { return luaL_loadstring_orig((lua_State_orig*)L, s); }
inline int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc) { return lua_pcall_orig((lua_State_orig*)L, nargs, nresults, errfunc); }
inline const char* lua_tostring(lua_State* L, int idx) { return lua_tostring_orig((lua_State_orig*)L, idx); }
inline void lua_pop(lua_State* L, int n) { lua_pop_orig((lua_State_orig*)L, n); }
inline int lua_type(lua_State* L, int idx) { return lua_type_orig((lua_State_orig*)L, idx); }
inline const char* lua_typename(lua_State* L, int tp) { return lua_typename_orig((lua_State_orig*)L, tp); }
inline double lua_tonumber(lua_State* L, int idx) { return lua_tonumber_orig((lua_State_orig*)L, idx); }
inline int lua_toboolean(lua_State* L, int idx) { return lua_toboolean_orig((lua_State_orig*)L, idx); }
inline void* lua_touserdata(lua_State* L, int idx) { return lua_touserdata_orig((lua_State_orig*)L, idx); }
inline void lua_getglobal(lua_State* L, const char* name) { lua_getglobal_orig((lua_State_orig*)L, name); }
inline void lua_setglobal(lua_State* L, const char* name) { lua_setglobal_orig((lua_State_orig*)L, name); }
inline void lua_pushnil(lua_State* L) { lua_pushnil_orig((lua_State_orig*)L); }
inline void lua_pushnumber(lua_State* L, double n) { lua_pushnumber_orig((lua_State_orig*)L, n); }
inline void lua_pushstring(lua_State* L, const char* s) { lua_pushstring_orig((lua_State_orig*)L, s); }
inline int lua_next(lua_State* L, int idx) { return lua_next_orig((lua_State_orig*)L, idx); }
inline void lua_getfield(lua_State* L, int idx, const char* k) { lua_getfield_orig((lua_State_orig*)L, idx, k); }
inline int lua_getmetatable(lua_State* L, int idx) { return lua_getmetatable_orig((lua_State_orig*)L, idx); }
inline size_t lua_objlen(lua_State* L, int idx) { return lua_objlen_orig((lua_State_orig*)L, idx); }
inline int lua_gettop(lua_State* L) { return lua_gettop_orig((lua_State_orig*)L); }
inline void lua_settop(lua_State* L, int idx) { lua_settop_orig((lua_State_orig*)L, idx); }
inline void lua_pushvalue(lua_State* L, int idx) { lua_pushvalue_orig((lua_State_orig*)L, idx); }
inline void lua_insert(lua_State* L, int idx) { lua_insert_orig((lua_State_orig*)L, idx); }
inline void lua_remove(lua_State* L, int idx) { lua_remove_orig((lua_State_orig*)L, idx); }
inline void lua_replace(lua_State* L, int idx) { lua_replace_orig((lua_State_orig*)L, idx); }
inline void lua_createtable(lua_State* L, int narr, int nrec) { lua_createtable_orig((lua_State_orig*)L, narr, nrec); }
inline void lua_setfield(lua_State* L, int idx, const char* k) { lua_setfield_orig((lua_State_orig*)L, idx, k); }
inline void lua_rawgeti(lua_State* L, int idx, int n) { lua_rawgeti_orig((lua_State_orig*)L, idx, n); }
inline void lua_rawseti(lua_State* L, int idx, int n) { lua_rawseti_orig((lua_State_orig*)L, idx, n); }
inline void lua_gettable(lua_State* L, int idx) { lua_gettable_orig((lua_State_orig*)L, idx); }
inline void lua_settable(lua_State* L, int idx) { lua_settable_orig((lua_State_orig*)L, idx); }
inline void lua_close(lua_State* L) { lua_close_orig((lua_State_orig*)L); }

#pragma pop_macro("LUA_H")
#pragma pop_macro("LUALIB_H")
#pragma pop_macro("LAUXLIB_H")

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/val.h>

    struct AsyncCallbackData {
        std::string code;
        void (*callback)(const LuaResult&);
    };

    static std::vector<AsyncCallbackData> asyncQueue;
#endif

class LuaStateManager {
private:
    lua_State* L;
    bool ownsState;

public:
    LuaStateManager() : L(nullptr), ownsState(true) {
        L = luaL_newstate();
        if (!L) {
            throw std::runtime_error("Failed to create Lua state (Lua).");
        }

        luaL_openlibs(L);
        restrictEnvironment();
    }

    explicit LuaStateManager(lua_State* state) : L(state), ownsState(false) {}

    ~LuaStateManager() {
        if (ownsState && L) {
            lua_close(L);
        }
    }

    lua_State* getState() {
        return L;
    }

    void execute(const std::string& code, int numReturns = 0) {
        if (code.empty()) {
            return;
        }

        int result = luaL_loadstring(L, code.c_str());
        if (result != LUA_OK) {
            std::string error = lua_tostring(L, -1);
            lua_pop(L, 1);
            throw std::runtime_error("Lua compilation error: " + error);
        }

        result = lua_pcall(L, 0, numReturns, 0);
        if (result != LUA_OK) {
            std::string error = lua_tostring(L, -1);
            lua_pop(L, 1);
            throw std::runtime_error("Lua runtime error: " + error);
        }
    }

    std::string getTopValueAsString() {
        return valueToString(L, -1);
    }

    int getTopValueType() {
        return lua_type(L, -1);
    }

    void pop(int n = 1) {
        lua_pop(L, n);
    }

private:
    void restrictEnvironment() {
        const char* dangerous_globals[] = {
            "dofile", "loadfile", "io", "os", "package", "debug",
            "collectgarbage", "getmetatable", "setmetatable",
            "load", "loadstring", "rawget", "rawset", "rawequal",
            nullptr
        };

        for (int i = 0; dangerous_globals[i] != nullptr; i++) {
            lua_pushnil(L);
            lua_setglobal(L, dangerous_globals[i]);
        }

        lua_getglobal(L, "string");
        lua_setglobal(L, "string");

        lua_getglobal(L, "table");
        lua_setglobal(L, "table");

        lua_getglobal(L, "math");
        lua_setglobal(L, "math");

        lua_getglobal(L, "utf8");
        lua_setglobal(L, "utf8");

        lua_getglobal(L, "coroutine");
        lua_setglobal(L, "coroutine");
    }

    std::string valueToString(lua_State* state, int index) {
        int type = lua_type(state, index);
        switch (type) {
            case LUA_TSTRING:
                return lua_tostring(state, index);
            case LUA_TNUMBER:
                return std::to_string(lua_tonumber(state, index));
            case LUA_TBOOLEAN:
                return lua_toboolean(state, index) ? "true" : "false";
            case LUA_TNIL:
                return "null";
            default:
                return lua_typename(state, type);
        }
    }
};

static std::string luaTableToJSON(lua_State* L, int index) {
    std::string result = "{";
    bool first = true;

    int baseIndex = (index < 0) ? lua_gettop(L) + index + 1 : index;

    lua_pushnil(L);
    while (lua_next(L, baseIndex) != 0) {
        if (!first) result += ",";
        first = false;

        int keyType = lua_type(L, -2);
        if (keyType == LUA_TSTRING) {
            const char* key = lua_tostring(L, -2);
            result += "\"" + std::string(key) + "\":";
        } else if (keyType == LUA_TNUMBER) {
            double keyNum = lua_tonumber(L, -2);
            result += std::to_string(keyNum) + ":";
        } else {
            result += "\"key\":";
        }

        int valType = lua_type(L, -1);
        switch (valType) {
            case LUA_TSTRING: {
                const char* val = lua_tostring(L, -1);
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
            case LUA_TNUMBER:
                result += std::to_string(lua_tonumber(L, -1));
                break;
            case LUA_TBOOLEAN:
                result += lua_toboolean(L, -1) ? "true" : "false";
                break;
            case LUA_TNIL:
                result += "null";
                break;
            case LUA_TTABLE:
                result += luaTableToJSON(L, lua_gettop(L));
                break;
            default:
                result += "\"" + std::string(lua_typename(L, valType)) + "\"";
                break;
        }

        lua_pop(L, 1);
    }

    result += "}";
    return result;
}

static std::string luaArrayToJSON(lua_State* L, int index) {
    std::string result = "[";
    bool first = true;

    int baseIndex = (index < 0) ? lua_gettop(L) + index + 1 : index;
    int len = lua_objlen(L, baseIndex);

    for (int i = 1; i <= len; i++) {
        if (!first) result += ",";
        first = false;

        lua_pushnumber(L, i);
        lua_gettable(L, baseIndex);

        int valType = lua_type(L, -1);
        switch (valType) {
            case LUA_TSTRING: {
                const char* val = lua_tostring(L, -1);
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
            case LUA_TNUMBER:
                result += std::to_string(lua_tonumber(L, -1));
                break;
            case LUA_TBOOLEAN:
                result += lua_toboolean(L, -1) ? "true" : "false";
                break;
            case LUA_TNIL:
                result += "null";
                break;
            case LUA_TTABLE:
                if (lua_objlen(L, -1) > 0) {
                    result += luaArrayToJSON(L, lua_gettop(L));
                } else {
                    result += luaTableToJSON(L, lua_gettop(L));
                }
                break;
            default:
                result += "\"" + std::string(lua_typename(L, valType)) + "\"";
                break;
        }

        lua_pop(L, 1);
    }

    result += "]";
    return result;
}

void RunLua::runScript(const std::string& code) {
    LuaStateManager luaManager;
    lua_State* L = luaManager.getState();

    if (code.empty()) {
        return;
    }

    int result = luaL_loadstring(L, code.c_str());
    if (result != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        lua_pop(L, 1);
        throw std::runtime_error(std::string("Lua compilation error: ") + (error ? error : "Unknown Lua compilation error"));
    }

    result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (result != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        lua_pop(L, 1);
        throw std::runtime_error(std::string("Lua runtime error: ") + (error ? error : "Unknown Lua runtime error"));
    }
}

std::pair<std::string, int> RunLua::runScriptWithResult(const std::string& code) {
    LuaResult result = runScriptWithDetailedResult(code);
    return {result.value, result.type};
}

LuaResult RunLua::runScriptWithDetailedResult(const std::string& code) {
    LuaStateManager luaManager;
    lua_State* L = luaManager.getState();

    int result = luaL_loadstring(L, code.c_str());
    if (result != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        std::string errMsg = error ? error : "Unknown Lua compilation error";
        lua_pop(L, 1);
        return LuaResult(errMsg);
    }

    result = lua_pcall(L, 0, 1, 0);
    if (result != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        std::string errMsg = error ? error : "Unknown Lua runtime error";
        lua_pop(L, 1);
        return LuaResult(errMsg);
    }

    std::string output;
    int outputType = 0;
    int type = lua_type(L, -1);

    switch (type) {
        case LUA_TNUMBER:
            output = std::to_string(lua_tonumber(L, -1));
            outputType = 1;
            break;
        case LUA_TBOOLEAN:
            output = lua_toboolean(L, -1) ? "true" : "false";
            outputType = 2;
            break;
        case LUA_TNIL:
            output = "null";
            outputType = 3;
            break;
        case LUA_TSTRING:
            output = lua_tostring(L, -1);
            outputType = 0;
            break;
        case LUA_TTABLE: {
            int len = lua_objlen(L, -1);
            if (len > 0) {
                output = luaArrayToJSON(L, -1);
                outputType = 5; // array
            } else {
                output = luaTableToJSON(L, -1);
                outputType = 4; // object
            }
            break;
        }
        case LUA_TFUNCTION:
            output = "[Lua Function]";
            outputType = 6;
            break;
        case LUA_TTHREAD:
            output = "[Lua Thread]";
            outputType = 7;
            break;
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA: {
            void* ptr = lua_touserdata(L, -1);
            std::stringstream ss;
            ss << "[Lua UserData: 0x" << std::hex << reinterpret_cast<uintptr_t>(ptr) << "]";
            output = ss.str();
            outputType = 8;
            break;
        }
        default:
            output = "[Lua " + std::string(lua_typename(L, type)) + "]";
            outputType = 0;
            break;
    }

    lua_pop(L, 1);
    return LuaResult(output, outputType);
}

bool RunLua::compileScript(const std::string& code, std::string& error) {
    LuaStateManager luaManager;
    lua_State* L = luaManager.getState();

    int result = luaL_loadstring(L, code.c_str());
    if (result != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        error = err ? err : "Compilation failed";
        lua_pop(L, 1);
        return false;
    }

    lua_pop(L, 1);
    return true;
}

LuaResult RunLua::runScriptWithGlobals(const std::string& code, const std::unordered_map<std::string, std::string>& globals) {
    LuaStateManager luaManager;
    lua_State* L = luaManager.getState();

    for (const auto& [key, value] : globals) {
        lua_pushstring(L, value.c_str());
        lua_setglobal(L, key.c_str());
    }

    int result = luaL_loadstring(L, code.c_str());
    if (result != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        std::string errMsg = error ? error : "Unknown Lua compilation error";
        lua_pop(L, 1);
        return LuaResult(errMsg);
    }

    result = lua_pcall(L, 0, 1, 0);
    if (result != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        std::string errMsg = error ? error : "Unknown Lua runtime error";
        lua_pop(L, 1);
        return LuaResult(errMsg);
    }

    std::string output;
    int outputType = 0;
    int type = lua_type(L, -1);

    switch (type) {
        case LUA_TNUMBER:
            output = std::to_string(lua_tonumber(L, -1));
            outputType = 1;
            break;
        case LUA_TBOOLEAN:
            output = lua_toboolean(L, -1) ? "true" : "false";
            outputType = 2;
            break;
        case LUA_TNIL:
            output = "null";
            outputType = 3;
            break;
        case LUA_TSTRING:
            output = lua_tostring(L, -1);
            outputType = 0;
            break;
        default:
            output = lua_typename(L, type);
            outputType = 0;
            break;
    }

    lua_pop(L, 1);
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
