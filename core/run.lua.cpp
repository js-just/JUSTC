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

void RunLua::runScript(const std::string& code) {
    lua_State *L = lua_newstate([](void *ud, void *ptr, size_t osize, size_t nsize) {
        (void)ud; (void)osize;
        if (nsize == 0) {
            free(ptr);
            return nullptr;
        } else {
            return realloc(ptr, nsize);
        }
    }, nullptr);

    if (!L) throw std::runtime_error("lua_newstate failed");

    luaL_requiref(L, "_G", luaopen_base, 1);
    lua_pop(L, 1);

    if (luaL_loadstring(L, code.c_str()) != LUA_OK) {
        std::string err = lua_tostring(L, -1);
        lua_close(L);
        throw std::runtime_error("Load: " + err);
    }

    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        std::string err = lua_tostring(L, -1);
        lua_close(L);
        throw std::runtime_error("Runtime: " + err);
    }

    lua_close(L);
}
