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

#include <iostream>
#include <fstream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "json_serializer.h"

void printUsage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "  justc <file.justc>                    - Execute JUSTC file" << std::endl;
    std::cout << "  justc -c \"code\"                   - Execute JUSTC code" << std::endl;
    std::cout << "  justc --lexer <file.justc>            - Run lexer only (file)" << std::endl;
    std::cout << "  justc --lexer -c \"code\"           - Run lexer only" << std::endl;
    std::cout << "  justc --help                          - Show this command list" << std::endl;
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to read the file: " + filename);
    }
    return std::string((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
}
void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to write to file: " + filename);
    }
    file << content;
    if (!file.good()) {
        throw std::runtime_error("Error occurred while writing to file: " + filename);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    try {
        std::string mode = "execute";
        std::string input;
        std::string output;
        bool outputToConsole = false;
        
        // parse arguments
        bool gotFileOrCode = false;
        bool outputToFile = false;
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printUsage();
                return 0;
            }
            else if (arg == "--lexer") {
                mode = "lexer";
            }
            else if (arg == "--result") {
                outputToConsole = true;
            }
            else if (arg == "-c" && i + 1 < argc) {
                input = argv[++i];
                gotFileOrCode = true;
            }
            else if (arg[0] != '-' && gotFileOrCode) {
                output = arg;
                outputToFile = true;
            }
            else if (arg[0] != '-') {
                input = readFile(arg);
                gotFileOrCode = true;
            }
        }
        
        if (input.empty()) {
            std::cerr << "Error: No input provided" << std::endl;
            printUsage();
            return 1;
        }
        
        std::string json;
        if (mode == "lexer") {
            auto lexerResult = Lexer::parse(input);
            json = JsonSerializer::serialize(lexerResult.second, lexerResult.first);
        }
        else {
            auto lexerResult = Lexer::parse(input);
            auto parseResult = Parser::parseTokens(lexerResult.second);
            json = JsonSerializer::serialize(parseResult);
        }

        if (outputToConsole) {
            std::cout << json << std::endl;
        }
        if (outputToFile) {
            writeFile(output, json);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
