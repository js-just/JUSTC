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

#include "codec.hpp"

#include <sstream>
#include <stdexcept>

namespace
{

bool startsWith(
    const std::string& s,
    const char* prefix
){

    size_t len=
        std::strlen(prefix);

    return
        s.size()>=len &&
        s.compare(
            0,
            len,
            prefix
        )==0;
}

}

namespace JUSTC::Codec
{

static std::string escape(
    const std::string& s
){

    std::string out;

    for(char c:s)
    {
        switch(c)
        {
            case '\\':
                out+="\\\\";
                break;

            case ';':
                out+="\\;";
                break;

            case ':':
                out+="\\:";
                break;

            case '=':
                out+="\\=";
                break;

            default:
                out+=c;
        }
    }

    return out;
}

static std::string unescape(
    const std::string& s
){

    std::string out;

    bool esc=false;

    for(char c:s)
    {
        if(esc)
        {
            out+=c;
            esc=false;
            continue;
        }

        if(c=='\\')
        {
            esc=true;
            continue;
        }

        out+=c;
    }

    return out;
}

static std::string encodeValue(
    const Value&
);

static Value decodeValue(
    const std::string&
);

static std::string encodeObject(
    const Object& obj
){

    std::string out=
        "o{";

    for(auto& [k,v]:obj)
    {
        out+="s:";
        out+=escape(k);

        out+="=";

        out+=encodeValue(v);

        out+=";";
    }

    out+="}";

    return out;
}

static std::string encodeArray(
    const Array& arr
){

    std::string out=
        "a[";

    for(auto& v:arr)
    {
        out+=
            encodeValue(v);

        out+=";";
    }

    out+="]";

    return out;
}

static std::string encodeValue(
    const Value& v
){

    if(v.isNull())
        return "x";

    if(v.isBool())
        return std::string(
            "b:"
        )+
        (
            v.get<bool>()
            ?"1":"0"
        );

    if(v.isNumber())
    {
        std::ostringstream ss;

        ss<<
            v.get<double>();

        return
            "n:"+
            ss.str();
    }

    if(v.isString())
    {
        return
            "s:"+
            escape(
                v.get<std::string>()
            );
    }

    if(v.isArray())
    {
        return encodeArray(
            v.get<Array>()
        );
    }

    if(v.isObject())
    {
        return encodeObject(
            v.get<Object>()
        );
    }

    return "x";
}

static size_t findEnd(
    const std::string& s,
    size_t pos
){

    int depth=0;

    for(size_t i=pos;i<s.size();i++)
    {
        if(s[i]=='{'||s[i]=='[')
            depth++;

        if(s[i]=='}'||s[i]==']')
            depth--;

        if(depth==0)
            return i;
    }

    return std::string::npos;
}

static Value decodeValue(
    const std::string& s
){

    if(s=="x")
        return {};

    if(
        startsWith(s, "b:")
    ){
        return
            s[2]=='1';
    }

    if(
        startsWith(s, "n:")
    ){
        return
            std::stod(
                s.substr(2)
            );
    }

    if(
        startsWith(s, "s:")
    ){
        return
            unescape(
                s.substr(2)
            );
    }

    if(
        startsWith(s, "a[")
    ){

        Array arr;

        size_t i=2;

        while(
            s[i]!=']'
        ){

            size_t end=i;

            int depth=0;

            while(true)
            {
                char c=s[end];

                if(
                    c=='{'||
                    c=='['
                )
                    depth++;

                if(
                    c=='}'||
                    c==']'
                )
                    depth--;

                if(
                    c==';' &&
                    depth==0
                )
                    break;

                end++;
            }

            arr.push_back(
                decodeValue(
                    s.substr(
                        i,
                        end-i
                    )
                )
            );

            i=end+1;
        }

        return arr;
    }

    if(
        startsWith(s, "o{")
    ){

        Object obj;

        size_t i=2;

        while(
            s[i]!='}'
        ){

            size_t eq=
                s.find(
                    '=',
                    i
                );

            std::string key=
                unescape(
                    s.substr(
                        i+2,
                        eq-i-2
                    )
                );

            size_t valStart=
                eq+1;

            size_t end=
                valStart;

            int depth=0;

            while(true)
            {
                char c=s[end];

                if(
                    c=='{'||
                    c=='['
                )
                    depth++;

                if(
                    c=='}'||
                    c==']'
                )
                    depth--;

                if(
                    c==';' &&
                    depth==0
                )
                    break;

                end++;
            }

            obj[key]=
                decodeValue(
                    s.substr(
                        valStart,
                        end-valStart
                    )
                );

            i=end+1;
        }

        return obj;
    }

    return {};
}

std::string encode(
    const Object& obj
){

    return encodeObject(
        obj
    );
}

Object decode(
    const std::string& s
){

    return
        decodeValue(
            s
        )
        .get<Object>();
}

}
