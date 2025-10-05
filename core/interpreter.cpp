#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <sstream>

#ifdef __EMSCRIPTEN__
#include <emscripten/fetch.h>

void downloadSucceeded(emscripten_fetch_t *fetch) {
    std::cout << "HTTP request succeeded: " << fetch->url << std::endl;
}

void downloadFailed(emscripten_fetch_t *fetch) {
    std::cout << "HTTP request failed: " << fetch->url << std::endl;
}

#elif _WIN32
    #include <windows.h>
    #include <winhttp.h>
    #pragma comment(lib, "winhttp.lib")
#else
    #include <curl/curl.h>
#endif

ParseResult Interpreter::interpret(const std::string& code) {
    auto lexerResult = Lexer::parse(code);
    auto parseResult = Parser::parseTokens(lexerResult.second);
    
    processHttpRequests(parseResult);
    
    return parseResult;
}

void Interpreter::processHttpRequests(ParseResult& result) {
    for (auto& pair : result.returnValues) {
        if (pair.second.type == DataType::VARIABLE) {
            std::string varName = pair.second.string_value;
        }
        else if (pair.second.type == DataType::LINK) {
            std::string url = pair.second.string_value;
        }
    }
}

#ifdef __EMSCRIPTEN__
// Browsers

std::string Interpreter::executeHttpRequest(const std::string& url) {
    return "";
}

#elif _WIN32
// Windows (WinHTTP)

std::string Interpreter::executeHttpRequest(const std::string& url) {
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

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string Interpreter::executeHttpRequest(const std::string& url) {
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

Value Interpreter::fetchHttpContent(const std::string& url, const std::string& expectedType) {
    Value result;
    
    try {
        std::string content = executeHttpRequest(url);
        
        if (expectedType == "JSON") {
            result.type = DataType::JSON_OBJECT;
            result.string_value = content;
            // TODO: parse JSON
        } else if (expectedType == "TEXT" || expectedType == "HTTPTEXT") {
            result.type = DataType::STRING;
            result.string_value = content;
        } else if (expectedType == "JUSTC" || expectedType == "HTTPJUSTC") {
            result.type = DataType::JUSTC_OBJECT;
            result.string_value = content;
            // TODO: parse JUSTC
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
