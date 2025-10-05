#include "keywords.h"

const std::vector<std::string> keywords = {
    "TYPE", "GLOBAL", "LOCAL", "STRICT", "ALL", "JSON", 
    "HTTPJSON", "HTTPTEXT", "HTTPJUSTC", "JUSTC", 
    "IMPORT", "EXPORT", "COPY", "REQUIRE", "ENV", 
    "CONFIG", "RUN", "VALUE", "FILE", "STAT", "STRING", 
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
    {"F", "FILE"}, {"FS", "STAT"}, {"ST", "STRING"}, {"SL", "LINK"},
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
    {"HJC", "HTTPJUSTC"}, {"CF", "CONFIG"}, {"SZ", "STAT"}, 
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
