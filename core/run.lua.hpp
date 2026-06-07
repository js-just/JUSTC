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

#ifndef RUN_LUA_HPP
#define RUN_LUA_HPP

#include <string>
#include <utility>
#include <memory>
#include <vector>
#include <unordered_map>

struct LuaResult {
    std::string value;
    int type; // 0 = string, 1 = number, 2 = boolean, 3 = nil, 4 = table, 5 = function, 6 = thread, 7 = userdata
    bool isError;
    std::string errorMessage;

    LuaResult() : type(0), isError(false) {}
    LuaResult(const std::string& v, int t) : value(v), type(t), isError(false) {}
    LuaResult(const std::string& err) : value(err), type(0), isError(true), errorMessage(err) {}
};

class RunLua {
public:
    static void runScript(const std::string& code);
    static std::pair<std::string, int> runScriptWithResult(const std::string& code);
    static LuaResult runScriptWithDetailedResult(const std::string& code);
    static bool compileScript(const std::string& code, std::string& error);
    static LuaResult runScriptWithGlobals(const std::string& code, const std::unordered_map<std::string, std::string>& globals);
    static LuaResult runScriptWithTimeout(const std::string& code, int timeoutMs);

#ifdef __EMSCRIPTEN__
    static void runScriptAsync(const std::string& code, void (*callback)(const LuaResult&));
#endif

private:
    static void initializeLuaState(void* L);
    static void restrictEnvironment(void* L);
    static std::string valueToString(void* L, int index);
    static std::string tableToJSON(void* L, int index);
    static std::string arrayToJSON(void* L, int index);
    static void setGlobalVariables(void* L, const std::unordered_map<std::string, std::string>& globals);

    static void setupSandbox(void* L);
};

#endif
