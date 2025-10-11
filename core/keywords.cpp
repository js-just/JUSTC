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

#include "keywords.h"

const std::vector<std::string> keywords = {
    "TYPE", "GLOBAL", "LOCAL", "STRICT", "ALL", "JSON", 
    "HTTPJSON", "HTTPTEXT", "HTTPJUSTC", "JUSTC", 
    "IMPORT", "EXPORT", "COPY", "REQUIRE", "ENV", 
    "CONFIG", "RUN", "VALUE", "FILE", "SIZE", "STRING", 
    "LINK", "STRINGNUM", "STRINGB64", "STRINGBIN", 
    "STRINGHEX", "TYPEID", "TYPEOF", "OUTPUT", "RETURN",
    "V", "D", "SQ", "CU", "P", "M", "S", "C", "T", "N",
    "ECHO", "LOGFILE", "LOG", "PARSEJUSTC", "PARSEJSON",
    "SPECIFIED", "EVERYTHING", "DISABLED", "AS",
    "STRINGOCT", "NUMBER", "BINARY", "OCTAL", "BASE64",
    "HEXADECIMAL", "PARSEHOCON", "HTTPHOCON", "BASE32",
    "STRINGB32", "ABSOLUTE", "CEIL", "FLOOR", "ALLOW",
    "DISALLOW", "JAVASCRIPT"
};

const std::unordered_map<std::string, std::string> smallKeywords = {
    {"T", "TYPE"}, {"G", "GLOBAL"}, {"L", "LOCAL"}, {"SC", "STRICT"},
    {"A", "ALL"}, {"JS", "JSON"}, {"JC", "JUSTC"}, {"I", "IMPORT"},
    {"EX", "EXPORT"}, {"CP", "COPY"}, {"RQ", "REQUIRE"}, {"E", "ENV"},
    {"HJS", "HTTPJSON"}, {"HT", "HTTPTEXT"}, {"HJ", "HTTPJUSTC"},
    {"HH", "HTTPHOCON"}, {"CO", "CONFIG"}, {"R", "RUN"}, {"O", "VALUE"},
    {"F", "FILE"}, {"SZ", "SIZE"}, {"ST", "STRING"}, {"SL", "LINK"},
    {"SN", "STRINGNUM"}, {"S64", "STRINGB64"}, {"S02", "STRINGBIN"},
    {"S16", "STRINGHEX"}, {"ID", "TYPEID"}, {"OF", "TYPEOF"},
    {"OUT", "OUTPUT"}, {"RT", "RETURN"}, {"Q", "SQ"}, {"U", "CU"},
    {"PJ", "PARSEJUSTC"}, {"PJS", "PARSEJSON"}, {"SP", "SPECIFIED"},
    {"EV", "EVERYTHING"}, {"N", "DISABLED"}, 
    {"NUM", "NUMBER"}, {"SO", "STRINGOCT"}, {"B", "BINARY"}, 
    {"OC", "OCTAL"}, {"B64", "BASE64"}, {"HEX", "HEXADECIMAL"},
    {"PH", "PARSEHOCON"}, {"S32", "STRINGB32"}, {"B32", "BASE32"},
    {"AB", "ABSOLUTE"}, {"CE", "CEIL"}, {"FL", "FLOOR"}, {"AL", "ALLOW"},
    {"DL", "DISALLOW"}, {"J", "JAVASCRIPT"},

    {"HJC", "HTTPJUSTC"}, {"CF", "CONFIG"},
    {"S10", "STRINGNUM"}, {"SNUM", "STRINGNUM"}, {"TID", "TYPEID"},
    {"TO", "TYPEOF"}, {"PUT", "RETURN"}, {"PJC", "PARSEJUSTC"},
    {"SPEC", "SPECIFIED"}, {"EVERY", "EVERYTHING"}, {"DIS", "DISABLED"},
    {"SOCT", "STRINGOCT"}, {"S08", "STRINGOCT"}, {"BIN", "BINARY"},
    {"OCT", "OCTAL"}, {"HEXDEC", "HEXADECIMAL"}, {"B02", "BINARY"},
    {"B08", "OCTAL"}, {"B10", "NUMBER"}, {"B16", "HEXADECIMAL"},
    {"ABS", "ABSOLUTE"}
};

const std::unordered_map<std::string, std::string> bigKeywords = {
    {"SQRT", "V"}, {"ROOT", "V"}, {"DOUBLE", "D"}, {"SQUARE", "SQ"},
    {"CUBE", "CU"}, {"NEGATIVE", "N"}, {"SIN", "S"}, {"COS", "C"},
    {"TAN", "T"}, {"SINE", "S"}, {"COSINE", "C"}, {"TANGENT", "T"}
};
