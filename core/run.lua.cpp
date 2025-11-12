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

#include "run.lua.hpp"
#define LUA_IMPL
#include <minilua/minilua.h>
#include <string>
#include <stdexcept>

struct LuaStateDeleter {
    void operator()(lua_State* L) const {
        if (L) lua_close(L);
    }
};

void RunLua::runScript(const std::string& code) {
    std::unique_ptr<lua_State, LuaStateDeleter> L(luaL_newstate());
    if (!L)
        throw std::runtime_error("Failed to create Lua state");

    luaL_openlibs(L.get());

    if (luaL_loadstring(L.get(), code.c_str()) != LUA_OK) {
        std::string error_msg = lua_tostring(L.get(), -1);
        throw std::runtime_error("Lua load error: " + error_msg);
    }

    if (lua_pcall(L.get(), 0, 0, 0) != LUA_OK) {
        std::string error_msg = lua_tostring(L.get(), -1);
        throw std::runtime_error("Lua runtime error: " + error_msg);
    }
}
