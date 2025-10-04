#include <cstring>
#include <cstdlib>
#include "lexer.h"

extern "C" {
    char* lexer(const char* input) {
        if (input == nullptr) return nullptr;
        auto parsed = lexer::parse(input);
        return parsed;
    }
}
