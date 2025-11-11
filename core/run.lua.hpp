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

#ifndef RUN_LUA_HPP
#define RUN_LUA_HPP

#include <string>

class RunLua {
public:
    /**
     * @brief Execute Luau code from a string
     *
     * @param code The Luau code to execute
     * @return std::string Result of execution or error message
     */
    static std::string runScript(const std::string& code);

    /**
     * @brief Process Luau blocks in the format <<--[[Lua code here]]>>
     *
     * @param luaCode The Luau code extracted from the block
     * @return std::string Result of execution
     */
    static std::string processLuauBlock(const std::string& luaCode);

    /**
     * @brief Initialize the Luau VM (called automatically, but can be called manually)
     */
    static void initialize();

    /**
     * @brief Cleanup the Luau VM resources
     */
    static void cleanup();

private:
    static bool isInitialized;
};

#endif
