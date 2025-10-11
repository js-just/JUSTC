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

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/fetch.h>

struct FetchResult {
    std::string data;
    bool completed = false;
    int status = 0;
};

void fetchSucceeded(emscripten_fetch_t *fetch) {
    FetchResult* result = (FetchResult*)fetch->userData;
    result->data = std::string(fetch->data, fetch->numBytes);
    result->status = fetch->status;
    result->completed = true;
}

void fetchFailed(emscripten_fetch_t *fetch) {
    FetchResult* result = (FetchResult*)fetch->userData;
    result->data = "HTTP Error: " + std::to_string(fetch->status);
    result->status = fetch->status;
    result->completed = true;
    std::runtime_error(result->data);
}

std::string Fetch::executeHttpRequest(const std::string& url) {
    FetchResult result;
    
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = fetchSucceeded;
    attr.onerror = fetchFailed;
    attr.userData = &result;
    
    const char* headers[] = {
        "Accept", "*/*",
        nullptr
    };
    attr.requestHeaders = headers;
    
    emscripten_fetch_t *fetch = emscripten_fetch(&attr, url.c_str());
    
    int maxWait = 100000; // 100s
    int waited = 0;
    while (!result.completed && waited < maxWait) {
        emscripten_sleep(100);
        waited += 100;
    }
    
    if (!result.completed) {
        emscripten_fetch_close(fetch);
        return "HTTP Error: Timeout";
    }
    
    emscripten_fetch_close(fetch);
    
    if (result.status == 200) {
        return result.data;
    } else {
        return "HTTP Error: " + std::to_string(result.status) + " - " + result.data;
    }
}

#elif _WIN32
// Windows (WinHTTP)

#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

std::string Fetch::executeHttpRequest(const std::string& url) {
    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    std::string result;
    
    try {
        URL_COMPONENTS urlComp;
        ZeroMemory(&urlComp, sizeof(urlComp));
        urlComp.dwStructSize = sizeof(urlComp);
        
        urlComp.dwSchemeLength = -1;
        urlComp.dwHostNameLength = -1;
        urlComp.dwUrlPathLength = -1;
        urlComp.dwExtraInfoLength = -1;
        
        if (!WinHttpCrackUrl(url.c_str(), url.length(), 0, &urlComp)) {
            throw std::runtime_error("Failed to parse URL");
        }
        
        std::wstring hostName(urlComp.lpszHostName, urlComp.dwHostNameLength);
        std::wstring urlPath(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
        
        hSession = WinHttpOpen(L"JUSTC/1.0", 
                              WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                              WINHTTP_NO_PROXY_NAME, 
                              WINHTTP_NO_PROXY_BYPASS, 0);
        
        if (!hSession) {
            throw std::runtime_error("Failed to create HTTP session");
        }
        
        hConnect = WinHttpConnect(hSession, hostName.c_str(),
                                 urlComp.nPort, 0);
        
        if (!hConnect) {
            throw std::runtime_error("Failed to connect to host");
        }
        
        hRequest = WinHttpOpenRequest(hConnect, L"GET", urlPath.c_str(),
                                     NULL, WINHTTP_NO_REFERER, 
                                     WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
        
        if (!hRequest) {
            throw std::runtime_error("Failed to create HTTP request");
        }
        
        if (!WinHttpSendRequest(hRequest, 
                               WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                               WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
            throw std::runtime_error("Failed to send HTTP request");
        }
        
        if (!WinHttpReceiveResponse(hRequest, NULL)) {
            throw std::runtime_error("Failed to receive HTTP response");
        }
        
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        char* pszOutBuffer;
        
        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                throw std::runtime_error("Error in WinHttpQueryDataAvailable");
            }
            
            if (dwSize == 0) break;
            
            pszOutBuffer = new char[dwSize + 1];
            if (!pszOutBuffer) {
                throw std::runtime_error("Out of memory");
            }
            
            ZeroMemory(pszOutBuffer, dwSize + 1);
            
            if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, 
                                dwSize, &dwDownloaded)) {
                delete[] pszOutBuffer;
                throw std::runtime_error("Error in WinHttpReadData");
            }
            
            result.append(pszOutBuffer, dwDownloaded);
            delete[] pszOutBuffer;
            
        } while (dwSize > 0);
        
    } catch (const std::exception& e) {
        result = "HTTP Error: " + std::string(e.what());
    }
    
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
    
    return result;
}

#else
// Linux/macOS (libcurl)

#include <curl/curl.h>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string Fetch::executeHttpRequest(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "JUSTC/1.0");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        
        res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            readBuffer = "HTTP Error: " + std::string(curl_easy_strerror(res));
        }
        
        curl_easy_cleanup(curl);
    } else {
        readBuffer = "HTTP Error: Failed to initialize curl";
    }
    
    return readBuffer;
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

Value Fetch::fetchHttpContent(const std::string& url, const std::string& expectedType) {
    Value result;
    
    try {
        std::string content = executeHttpRequest(url);

        if (content.empty()) {
            result.type = DataType::ERROR;
            result.string_value = "Empty response from: " + url;
            return result;
        }
        
        if (expectedType == "JSON" || expectedType == "HTTPJSON") {
            result.type = DataType::JSON_OBJECT;
            result.string_value = content;
        } else if (expectedType == "TEXT" || expectedType == "HTTPTEXT") {
            result.type = DataType::STRING;
            result.string_value = content;
        } else if (expectedType == "JUSTC" || expectedType == "HTTPJUSTC") {
            result.type = DataType::JUSTC_OBJECT;
            result.string_value = content;
        } else {
            result.type = DataType::STRING;
            result.string_value = content;
        }
        
    } catch (const std::exception& e) {
        result.type = DataType::ERROR;
        result.string_value = "HTTP request failed: " + std::string(e.what());
    }
    
    return result;
}

Value Fetch::httpGet(const std::string& url, const std::string& format) {
    if (url == "test") {
        Value result;
        result.type = DataType::STRING;
        result.string_value = "Just an Ultimate Site Tool Configuration language";
        return result;
    }
    return fetchHttpContent(url, format);
}
