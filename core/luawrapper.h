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

#ifndef LUA_WRAPPER_H
#define LUA_WRAPPER_H

#include <string>
#include <utility>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;

void* lua_wrapper_newstate(void);
void lua_wrapper_close(void* L);
int lua_wrapper_loadstring(void* L, const char* s);
int lua_wrapper_pcall(void* L, int nargs, int nresults, int errfunc);
const char* lua_wrapper_tostring(void* L, int idx);
void lua_wrapper_pop(void* L, int n);
int lua_wrapper_type(void* L, int idx);
const char* lua_wrapper_typename(void* L, int tp);
double lua_wrapper_tonumber(void* L, int idx);
int lua_wrapper_toboolean(void* L, int idx);
void* lua_wrapper_touserdata(void* L, int idx);
void lua_wrapper_getglobal(void* L, const char* name);
void lua_wrapper_setglobal(void* L, const char* name);
void lua_wrapper_pushnil(void* L);
void lua_wrapper_pushnumber(void* L, double n);
void lua_wrapper_pushstring(void* L, const char* s);
int lua_wrapper_next(void* L, int idx);
void lua_wrapper_getfield(void* L, int idx, const char* k);
int lua_wrapper_getmetatable(void* L, int idx);
size_t lua_wrapper_objlen(void* L, int idx);
int lua_wrapper_gettop(void* L);
void lua_wrapper_settop(void* L, int idx);
void lua_wrapper_createtable(void* L, int narr, int nrec);
void lua_wrapper_rawgeti(void* L, int idx, int n);

void lua_wrapper_openlibs(void* L);

#ifdef __cplusplus
}
#endif

#endif
