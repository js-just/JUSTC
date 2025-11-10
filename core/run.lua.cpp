#include "run.lua.hpp"
#include <iostream>

Lua::Lua() {
    L = luaL_newstate();
    if (L) {
        luaL_openlibs(L);
        initJUSTC();
    }
}

Lua::~Lua() {
    if (L) {
        lua_close(L);
    }
}

bool Lua::executeScript(const std::string& script) {
    if (!L) return false;

    int result = luaL_dostring(L, script.c_str());
    if (result != LUA_OK) {
        std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        return false;
    }
    return true;
}

bool Lua::executeFile(const std::string& filename) {
    if (!L) return false;

    int result = luaL_dofile(L, filename.c_str());
    if (result != LUA_OK) {
        std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        return false;
    }
    return true;
}

void Lua::registerFunction(const std::string& name, lua_CFunction func) {
    if (L) {
        lua_register(L, name.c_str(), func);
    }
}

void Lua::initJUSTC() {
    registerFunction("justc_version", [](lua_State* L) -> int {
        luacpp11::push(L, "1.0.0");
        return 1;
    });
}
