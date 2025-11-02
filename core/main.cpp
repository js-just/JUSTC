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
#include "json.hpp"
#include "version.h"
#include <cstring>

void printUsage() {
    std::cout << "" << std::endl;
    std::cout << "Just an Ultimate Site Tool Configuration language (JUSTC) v" + JUSTC_VERSION << std::endl;
    std::cout << "https://just.js.org/justc" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: justc   [ flags ( [ arguments ] ) ]   [ input.justc ] ( [ output.json ] )" << std::endl;
    std::cout << "       justc <file.justc>               - Execute JUSTC file" << std::endl;
    std::cout << "       justc <file.justc> <file.json>   - Execute JUSTC file and output to JSON file" << std::endl;
    std::cout << "       justc -e \"<code>\"                - Execute JUSTC code" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Flags:" << std::endl;
    std::cout << "  -e, --eval                            - Execute (Evaluate) script (not file)" << std::endl;
    std::cout << "  -h, --help                            - Print JUSTC command line options (this list)" << std::endl;
    std::cout << "  -E, --execute                         - Execute JUSTC from lexer output tokens as JSON" << std::endl;
    std::cout << "  -l, --lexer                           - Run lexer only / Tokenize" << std::endl;
    std::cout << "  -P, --parser                          - Run parser only / Parse JUSTC (not execute) from lexer output tokens as JSON" << std::endl;
    std::cout << "  -p, --parse                           - Parse JUSTC (not execute) / No HTTP requests, some commands will not be executed" << std::endl;
    std::cout << "  -r, --result                          - Print result" << std::endl;
    std::cout << "  -s <commit sha>, --sha <commit sha>   - Set commit SHA" << std::endl;
    std::cout << "  -v, --version                         - Print JUSTC version" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "\"Run parser only\" means that JUSTC (will not be executed) will only be compiled to JSON - no logs and/or HTTP requests." << std::endl;
    std::cout << "\"Run lexer only\" means that JUSTC won't be executed and/or parsed, JUSTC will only be tokenized." << std::endl;
    std::cout << "" << std::endl;
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
    if (argc < 1) {
        printUsage();
        return 1;
    }

    try {
        std::string mode = "execute";
        std::string input;
        std::string output;
        bool outputToConsole = false;
        bool executeJUSTC = true;
        bool helpandorversionflag = false;
        bool lexerTokensToParser = false;
        
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
                helpandorversionflag = true;
                printUsage();
            }
            else if (arg == "--lexer" || arg == "-l") {
                mode = "lexer";
            }
            else if (arg == "--result" || arg == "-r") {
                outputToConsole = true;
            }
            else if ((arg == "-e" || arg == "--eval") && i + 1 < argc) {
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
                helpandorversionflag = true;
                std::cout << JUSTC_VERSION << std::endl;
            }
            else if (arg == "--parse" || arg == "-p") {
                executeJUSTC = false;
            }
            else if (arg == "--parser" || arg == "-P") {
                lexerTokensToParser = true;
                mode = "parser";
            }
            else if (arg == "--execute" || arg == "-E") {
                lexerTokensToParser = true;
                mode = "parserExecute";
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
        
        if (input.empty() && !helpandorversionflag) {
            std::runtime_error("No input provided");
            return 1;
        }
        
        std::string json;
        if (mode == "lexer") {
            auto lexerResult = Lexer::parse(input);
            json = JsonSerializer::serialize(lexerResult.second, lexerResult.first);
        }
        else if (mode == "parser" || mode == "parserExecute") {
            std::vector<ParserToken> lexerResult;
            JsonParser::parseJsonTokens(input.c_str(), lexerResult);
            auto parseResult = Parser::parseTokens(lexerResult, mode == "parserExecute");
            json = JsonSerializer::serialize(parseResult);
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
        std::runtime_error(e.what());
        return 1;
    }
    
    return 0;
}
