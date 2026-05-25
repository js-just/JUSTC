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

#include "capi.hpp"
#include "lib.hpp"
#include "codec.hpp"

extern "C"
{

const char*
justc_version()
{
    return JUSTC_VERSION.c_str();
}

const char* justc_parse(
    const char* code,
    bool execute,
    bool async
){

    static std::string result;

    result=
        JUSTC::Codec::encode(
            JUSTC::API::parse(
                code,
                execute,
                async
            )
        );

    return result.c_str();
}

const char* justc_stringify(
    const char* code
){

    static std::string result;

    result=
        JUSTC::API::stringify(
            JUSTC::Codec::decode(
                code
            )
        );

    return result.c_str();
}

}
