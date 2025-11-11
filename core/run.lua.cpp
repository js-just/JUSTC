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
#include <Luau/Compiler.h>
#include <Luau/BytecodeBuilder.h>
#include <Luau/VirtualMachine.h>
#include <string>
#include <stdexcept>
#include <memory>

static std::unique_ptr<Luau::VM> luaVM = nullptr;
bool RunLua::isInitialized = false;

void RunLua::initialize() {
    if (!isInitialized) {
        luaVM = std::make_unique<Luau::VM>();
        isInitialized = true;
    }
}

void RunLua::cleanup() {
    if (isInitialized) {
        luaVM.reset();
        isInitialized = false;
    }
}

std::string RunLua::runScript(const std::string& code) {
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
}
