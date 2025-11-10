#pragma once

extern "C" {
    #include <lua.hpp>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include "../third-party/progschj/luacpp11.hpp"

class RunLua {
public:
    RunLua();
    ~RunLua();

    bool executeScript(const std::string& script);
    bool executeFile(const std::string& filename);

    void registerFunction(const std::string& name, lua_CFunction func);

    template<typename Func>
    void registerFunction(const std::string& name, Func&& func) {
        lua_pushcfunction(L, luacpp11::push_callable(L, std::forward<Func>(func)));
        lua_setglobal(L, name.c_str());
    }

    template<typename T>
    T getGlobal(const std::string& name) {
        lua_getglobal(L, name.c_str());
        T result = luacpp11::to<T>(L, -1);
        lua_pop(L, 1);
        return result;
    }

    template<typename T>
    void setGlobal(const std::string& name, T&& value) {
        luacpp11::push(L, std::forward<T>(value));
        lua_setglobal(L, name.c_str());
    }

private:
    lua_State* L;

    void initJUSTC();
};
