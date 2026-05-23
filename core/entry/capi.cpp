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

#include <cstring>
#include <cstdlib>

#include "capi.hpp"
#include "lib.hpp"

static char*
copyString(
    const std::string& s
){

    char* out=
        (char*)std::malloc(
            s.size()+1
        );

    memcpy(
        out,
        s.c_str(),
        s.size()+1
    );

    return out;
}

extern "C"
{

const char*
justc_version()
{
    return JUSTC_VERSION.c_str();
}

char* justc_parse(
    const char* code,
    bool execute,
    bool async
){

    auto obj=
        JUSTC::API::parse(
            code,
            execute,
            async
        );

    auto out=
        JUSTC::API::stringify(
            obj
        );

    return copyString(out);
}

char* justc_stringify(
    const char* code
){

    auto obj=
        JUSTC::API::parse(
            code
        );

    auto out=
        JUSTC::API::stringify(
            obj
        );

    return copyString(out);
}

void justc_free(
    char* ptr
){

    std::free(ptr);

}

}
