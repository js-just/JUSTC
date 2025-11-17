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

#include "fetch.h"
#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <sstream>
#include "version.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <utility>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

long getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}

std::pair<std::string, std::pair<std::string, std::string>> Fetch::executeHttpRequest(const std::string& url, const std::string& method, const std::string& body, const std::unordered_map<std::string, std::string>& headers) {
    std::string serialized_headers;
    for (const auto& pair : headers) {
        serialized_headers += pair.first + ":" + pair.second + "\n";
    }

    struct HttpResult {
        char* response_text;
        char* status_code;
        char* response_headers;
    };

    HttpResult* http_result = (HttpResult*)EM_ASM_INT({
        return Asyncify.handleSleep(function(wakeUp) {
            try {
                var url = UTF8ToString($0);
                var version = UTF8ToString($1);
                var method = UTF8ToString($2);
                var headersStr = UTF8ToString($3);
                var body = UTF8ToString($4);

                var isBrowser = (typeof window !== 'undefined') && (typeof navigator !== 'undefined');
                var xhr = new XMLHttpRequest();
                xhr.open(method, url, false);

                xhr.setRequestHeader('Accept', '*/*');
                if (!isBrowser) {
                    xhr.setRequestHeader('User-Agent', 'JUSTC/' + version);
                }
                xhr.setRequestHeader('X-JUSTC', 'JUSTC/' + version);

                var headers = headersStr.split('\n');
                for (var i = 0; i < headers.length; i++) {
                    var header = headers[i];
                    if (header.trim() === '') continue;

                    var headerArray = header.split(':');
                    if (headerArray.length >= 2) {
                        var key = headerArray[0].trim();
                        var value = headerArray.slice(1).join(':').trim();

                        var keyLower = key.toLowerCase();
                        if (keyLower === 'user-agent' || keyLower === 'x-justc') {
                            throw new Error('Attempt to set "' + key + '" header.');
                        }

                        try {
                            xhr.setRequestHeader(key, value);
                        } catch (e) {
                            console.warn('[JUSTC] (' + UTF8ToString($5) + ') HTTP: Failed to set header ' + key + ' with value ' + value + ': ' + e);
                        }
                    }
                }

                xhr.onload = function() {
                    var responseText = xhr.responseText;
                    var statusCode = xhr.status.toString();
                    var headersString = "";

                    headersString = xhr.getAllResponseHeaders();
                    var headersObject = {};
                    var headerLines = headersString.trim().split('\n');

                    for (var j = 0; j < headerLines.length; j++) {
                        var line = headerLines[j];
                        var parts = line.split(': ');
                        if (parts.length === 2) {
                            var key = parts[0].trim();
                            var value = parts[1].trim();
                            headersObject[key] = value;
                        }
                    }

                    headersString = "";
                    for (var key in headersObject) {
                        if (headersObject.hasOwnProperty(key)) {
                            headersString += key + ":" + headersObject[key] + "\n";
                        }
                    }
                    headersString = "";

                    if (xhr.status >= 400 && xhr.status < 600 && method != "HEAD") {
                        if (responseText.length < 1) {
                            throw new Error('Request failed with status ' + statusCode + '.');
                        } else {
                            console.warn('[JUSTC] (' + UTF8ToString($5) + ') HTTP: Request succeeded, but with status ' + statusCode);
                        }
                    }

                    var result = _malloc(sizeof(HttpResult));
                    var responseTextPtr = _malloc(lengthBytesUTF8(responseText) + 1);
                    var statusCodePtr = _malloc(lengthBytesUTF8(statusCode) + 1);
                    var headersPtr = _malloc(lengthBytesUTF8(headersString) + 1);

                    stringToUTF8(responseText, responseTextPtr, lengthBytesUTF8(responseText) + 1);
                    stringToUTF8(statusCode, statusCodePtr, lengthBytesUTF8(statusCode) + 1);
                    stringToUTF8(headersString, headersPtr, lengthBytesUTF8(headersString) + 1);

                    HEAPU32[result / 4] = responseTextPtr;
                    HEAPU32[result / 4 + 1] = statusCodePtr;
                    HEAPU32[result / 4 + 2] = headersPtr;

                    wakeUp(result);
                };

                xhr.onerror = function() {
                    throw new Error('Request failed with status ' + xhr.status + '.');
                };

                xhr.send(body.length > 0 ? body : undefined);
            } catch (e) {
                throw new Error('Request failed: ' + e);
            }
        });
    }, url.c_str(), JUSTC_VERSION.c_str(), method.c_str(), serialized_headers.c_str(), body.c_str(), Parser::getCurrentTimestamp().c_str());

    char* response_text_ptr = (char*)HEAPU32[http_result / 4];
    char* status_code_ptr = (char*)HEAPU32[http_result / 4 + 1];
    char* headers_ptr = (char*)HEAPU32[http_result / 4 + 2];

    std::string response_text(response_text_ptr);
    std::string status_code(status_code_ptr);
    std::string response_headers(headers_ptr);

    free(response_text_ptr);
    free(status_code_ptr);
    free(headers_ptr);
    free(http_result);

    return std::make_pair(response_text, std::make_pair(status_code, response_headers));
}

#else
#include <cpr/cpr.h>

std::pair<std::string, std::pair<std::string, std::string>> Fetch::executeHttpRequest(const std::string& url, const std::string& method, const std::string& body, const std::unordered_map<std::string, std::string>& headers) {
    cpr::Header reqHeaders = {
        {"User-Agent", "JUSTC/" + JUSTC_VERSION},
        {"Accept", "*/*"},
        {"X-JUSTC", "JUSTC/" + JUSTC_VERSION}
    };

    for (const auto& pair : headers) {
        std::string pairfirst = pair.first;
        std::string pf;
        pf.resize(pairfirst.size());
        std::transform(pairfirst.begin(), pairfirst.end(),
                pf.begin(),
                [](unsigned char c){ return std::tolower(c); });
        if (pf == "user-agent" || pf == "x-justc") throw std::runtime_error("HTTP: Attempt to set \"" + pair.first + "\" header.");
        reqHeaders[pair.first] = pair.second;
    }

    cpr::Response response;

    if (method == "GET") {
        response = cpr::Get(
            cpr::Url{url},
            reqHeaders,
            cpr::Timeout{10000}
        );
    } else if (method == "POST") {
        response = cpr::Post(
            cpr::Url{url},
            reqHeaders,
            cpr::Body{body},
            cpr::Timeout{10000}
        );
    } else if (method == "PUT") {
        response = cpr::Put(
            cpr::Url{url},
            reqHeaders,
            cpr::Body{body},
            cpr::Timeout{10000}
        );
    } else if (method == "PATCH") {
        response = cpr::Patch(
            cpr::Url{url},
            reqHeaders,
            cpr::Body{body},
            cpr::Timeout{10000}
        );
    } else if (method == "DELETE") {
        response = cpr::Delete(
            cpr::Url{url},
            reqHeaders,
            cpr::Timeout{10000}
        );
    } else if (method == "HEAD") {
        response = cpr::Head(
            cpr::Url{url},
            reqHeaders,
            cpr::Timeout{10000}
        );
    } else if (method == "OPTIONS") {
        response = cpr::Options(
            cpr::Url{url},
            reqHeaders,
            cpr::Timeout{10000}
        );
    } else {
        throw std::runtime_error("HTTP: Invalid method.");
    }

    if (response.status_code >= 400 && response.status_code < 600 && method != "HEAD") {
        if (response.text.empty()) {
            throw std::runtime_error("HTTP: Request failed with status " + std::to_string(response.status_code));
        } else {
            std::cerr << "[JUSTC] HTTP: Request succeeded, but with status " << response.status_code << std::endl;
        }
    }

    if (response.error) {
        throw std::runtime_error("HTTP: Request failed: " + response.error.message);
    }

    std::string responseHeaders;
    for (const auto& header : response.header) {
        responseHeaders += header.first + ":" + header.second + "\n";
    }
    std::pair<std::string, std::pair<std::string, std::string>> output(response.text, std::make_pair(std::to_string(response.status_code), responseHeaders));
    return output;
}

#endif

void Fetch::processHttpRequests(const ParseResult& result) {
    for (auto& pair : result.returnValues) {
        if (pair.second.type == DataType::VARIABLE) {
            std::string varName = pair.second.string_value;
        }
        else if (pair.second.type == DataType::LINK) {
            std::string url = pair.second.string_value;
        }
    }
}

Value Fetch::request(const std::string& url, const std::string& method, const std::unordered_map<std::string, std::string>& headers, const std::string& body) {
    Value result;

    try {
        auto content = executeHttpRequest(url, method, body, headers);

        result.type = DataType::JUSTC_OBJECT;
        result.object_value = std::unordered_map<std::string, Value>{
            {"text", Value{DataType::STRING, content.first}},
            {"status", Value{DataType::STRING, content.second.first}},
            {"headers", Value{DataType::STRING, content.second.second}}
        };
        result.name = "HTTP.Response";
    } catch (const std::exception& e) {
        std::runtime_error("HTTP request failed: " + std::string(e.what()));
    }

    return result;
}
