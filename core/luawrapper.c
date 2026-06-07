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

#include "luawrapper.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

void* lua_wrapper_newstate(void) {
    return luaL_newstate();
}

void lua_wrapper_close(void* L) {
    lua_close((lua_State*)L);
}

int lua_wrapper_loadstring(void* L, const char* s) {
    return luaL_loadstring((lua_State*)L, s);
}

int lua_wrapper_pcall(void* L, int nargs, int nresults, int errfunc) {
    return lua_pcall((lua_State*)L, nargs, nresults, errfunc);
}

const char* lua_wrapper_tostring(void* L, int idx) {
    return lua_tostring((lua_State*)L, idx);
}

void lua_wrapper_pop(void* L, int n) {
    lua_pop((lua_State*)L, n);
}

int lua_wrapper_type(void* L, int idx) {
    return lua_type((lua_State*)L, idx);
}

const char* lua_wrapper_typename(void* L, int tp) {
    return lua_typename((lua_State*)L, tp);
}

double lua_wrapper_tonumber(void* L, int idx) {
    return lua_tonumber((lua_State*)L, idx);
}

int lua_wrapper_toboolean(void* L, int idx) {
    return lua_toboolean((lua_State*)L, idx);
}

void* lua_wrapper_touserdata(void* L, int idx) {
    return lua_touserdata((lua_State*)L, idx);
}

void lua_wrapper_getglobal(void* L, const char* name) {
    lua_getglobal((lua_State*)L, name);
}

void lua_wrapper_setglobal(void* L, const char* name) {
    lua_setglobal((lua_State*)L, name);
}

void lua_wrapper_pushnil(void* L) {
    lua_pushnil((lua_State*)L);
}

void lua_wrapper_pushnumber(void* L, double n) {
    lua_pushnumber((lua_State*)L, n);
}

void lua_wrapper_pushstring(void* L, const char* s) {
    lua_pushstring((lua_State*)L, s);
}

int lua_wrapper_next(void* L, int idx) {
    return lua_next((lua_State*)L, idx);
}

void lua_wrapper_getfield(void* L, int idx, const char* k) {
    lua_getfield((lua_State*)L, idx, k);
}

int lua_wrapper_getmetatable(void* L, int idx) {
    return lua_getmetatable((lua_State*)L, idx);
}

size_t lua_wrapper_objlen(void* L, int idx) {
    #if LUA_VERSION_NUM >= 504
        return lua_rawlen((lua_State*)L, idx);
    #else
        return lua_objlen((lua_State*)L, idx);
    #endif
}

int lua_wrapper_gettop(void* L) {
    return lua_gettop((lua_State*)L);
}

void lua_wrapper_settop(void* L, int idx) {
    lua_settop((lua_State*)L, idx);
}

void lua_wrapper_createtable(void* L, int narr, int nrec) {
    lua_createtable((lua_State*)L, narr, nrec);
}

void lua_wrapper_rawgeti(void* L, int idx, int n) {
    lua_rawgeti((lua_State*)L, idx, n);
}

void lua_wrapper_openlibs(void* L) {
    luaL_openlibs((lua_State*)L);
}
