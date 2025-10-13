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
#include "version.h"

void printUsage() {
    std::cout << "JUSTC v" + JUSTC_VERSION << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Basic usage:" << std::endl;
    std::cout << "  justc <file.justc>                    - Execute JUSTC file" << std::endl;
    std::cout << "  justc <file.justc> <file.json>        - Execute JUSTC file and output to JSON file" << std::endl;
    std::cout << "  justc -c \"code\"                   - Execute JUSTC code" << std::endl;
    std::cout << "  justc --lexer <file.justc>            - Run lexer only (file)" << std::endl;
    std::cout << "  justc --lexer -c \"code\"           - Run lexer only" << std::endl;
    std::cout << "  justc --parse <file.justc>            - Run parser only (file)" << std::endl;
    std::cout << "  justc --parse -c \"code\"           - Run parser only" << std::endl;
    std::cout << "  justc --help                          - Show this command list" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Other flags:" << std::endl;
    std::cout << "  --result                              - Output to console" << std::endl;
    std::cout << "  --sha <commit sha>                    - Set commit SHA" << std::endl;
    std::cout << "  --version                             - Show JUSTC version" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Short flags:" << std::endl;
    std::cout << "  -h                                    - Same as --help" << std::endl;
    std::cout << "  -r                                    - Same as --result" << std::endl;
    std::cout << "  -p                                    - Same as --parse" << std::endl;
    std::cout << "  -l                                    - Same as --lexer" << std::endl;
    std::cout << "  -v                                    - Same as --version" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "\"Run parser only\" means that JUSTC (will not be executed) will only be compiled to JSON - no logs and/or HTTP requests." << std::endl;
    std::cout << "\"Run lexer only\" means that JUSTC won't be executed and/or parsed, JUSTC will only be tokenized." << std::endl;
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
        bool executeJUSTC = true;
        
        // parse arguments
        bool gotFileOrCode = false;
        bool outputToFile = false;
        bool waitingForVersion = false;
        bool waitingForConfig = false;
        bool waitingForCommitSHA = false;
        bool waitingForNav = false;
        bool waitingForPages = false;
        bool waitingForCSS = false;
        std::string justversion;
        std::string configpath;
        std::string commitSHA;
        std::string builtinvarNAV;
        std::string builtinvarPAGES;
        std::string builtinvarCSS;
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (waitingForVersion) {
                justversion = arg;
            }
            else if (waitingForConfig) {
                configpath = arg;
            }
            else if (waitingForCommitSHA) {
                commitSHA = arg;
            }
            else if (waitingForNav) {
                builtinvarNAV = arg;
            }
            else if (waitingForPages) {
                builtinvarPAGES = arg;
            }
            else if (waitingForCSS) {
                builtinvarCSS = arg;
            }
            
            else if (arg == "--help" || arg == "-h") {
                printUsage();
                return 0;
            }
            else if (arg == "--lexer" || arg == "-l") {
                mode = "lexer";
            }
            else if (arg == "--result" || arg == "-r") {
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
            else if (arg == "--sha") {
                waitingForCommitSHA = true;
            }
            else if (arg == "--version" || arg == "-v") {
                std::count << JUSTC_VERSION << std::endl;
            }
            else if (arg == "--parse" || arg == "-p") {
                executeJUSTC = false;
            }

            // hidden flags. IMPORTANT: DO NOT USE THESE FLAGS! THESE FLAGS ARE ONLY FOR JUST AN ULTIMATE SITE TOOL ENVIRONMENT.
            waitingForVersion = false;
            waitingForConfig = false;
            waitingForCommitSHA = false;
            waitingForNav = false;
            waitingForPages = false;
            waitingForCSS = false;
            if (arg == "--justversion") {
                waitingForVersion = true;
            }
            else if (arg == "--configfile") {
                waitingForConfig = true;
            }
            else if (arg == "--setnav") {
                waitingForNav = true;
            }
            else if (arg == "--setpages") {
                waitingForPages = true;
            }
            else if (arg == "--setcss") {
                waitingForCSS = true;
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
            auto parseResult = Parser::parseTokens(lexerResult.second, executeJUSTC);
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
