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

#pragma once

#include <string>
#include <map>
#include <vector>
#include <variant>

#ifdef _WIN32
    #ifdef JUSTC_BUILD
        #define JUSTC_API __declspec(dllexport)
    #else
        #define JUSTC_API __declspec(dllimport)
    #endif
#else
    #define JUSTC_API __attribute__((visibility("default")))
#endif

#include "capi.hpp"
#include "codec.hpp"

namespace JUSTC {

inline const char* Version()
{
    return justc_version();
}

class API {
public:

    static Object parse(
        const std::string& code,
        bool execute=true,
        bool async=false
    ){

        const char* ptr=
            justc_parse(
                code.c_str(),
                execute,
                async
            );

        std::string str = ptr;

        auto obj = Codec::decode(str);

        return obj;
    }

    static std::string stringify(
        const Object& obj
    ){

        auto encoded = Codec::encode(obj);

        const char* ptr=
            justc_stringify(
                encoded.c_str()
            );

        std::string out = ptr;

        return out;
    }

};

}
