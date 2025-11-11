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
#include <string>
#include <stdexcept>
#include <memory>

#ifndef NO_LUA_SUPPORT
#include <Luau/Compiler.h>
#include <Luau/BytecodeBuilder.h>
#include <Luau/VirtualMachine.h>
#endif

#ifndef NO_LUA_SUPPORT
static std::unique_ptr<Luau::VM> luaVM = nullptr;
#endif
bool RunLua::isInitialized = false;

void RunLua::initialize() {
#ifndef NO_LUA_SUPPORT
    if (!isInitialized) {
        luaVM = std::make_unique<Luau::VM>();
        isInitialized = true;
    }
#endif
}

void RunLua::cleanup() {
#ifndef NO_LUA_SUPPORT
    if (isInitialized) {
        luaVM.reset();
        isInitialized = false;
    }
#endif
}

std::string RunLua::runScript(const std::string& code) {
#ifdef NO_LUA_SUPPORT
    return "Luau support not compiled in";
#else
    initialize();

    try {
        std::string bytecode = Luau::compile(code);
        Luau::load(*luaVM, bytecode, "luau_script", 0);
        return "Luau code executed successfully";

    } catch (const Luau::CompileError& e) {
        return std::string("Luau compile error: ") + e.what();
    } catch (const std::exception& e) {
        return std::string("Luau runtime error: ") + e.what();
    }
#endif
}
