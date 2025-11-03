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

#include "to.xml.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include "parser.h"
#include <cmath>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

std::string XmlSerializer::escapeXmlString(const std::string& str) {
    std::stringstream ss;
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        switch (c) {
            case '&': ss << "&amp;"; break;
            case '<': ss << "&lt;"; break;
            case '>': ss << "&gt;"; break;
            case '"': ss << "&quot;"; break;
            case '\'': ss << "&apos;"; break;
            default: 
                if (static_cast<unsigned char>(c) >= 0x20 || c == '\n' || c == '\r' || c == '\t') {
                    ss << c;
                }
                break;
        }
        
        if (i > 100000) {
            #ifdef __EMSCRIPTEN__
            EM_ASM({
                console.warn("[JUSTC] (" + $0 + ") string is too long. It will be truncated in the XML output.");
            }, Parser::getCurrentTimestamp().c_str());
            #else
            std::cout << "JUSTC: Warning: string is too long. It will be truncated in the XML output." << std::endl;
            #endif
            break;
        }
    }
    return ss.str();
}

std::string XmlSerializer::valueToXml(const Value& value) {
    switch (value.type) {
        case DataType::NUMBER:
        case DataType::HEXADECIMAL:
        case DataType::BINARY:
        case DataType::OCTAL:
            if (value.number_value == std::floor(value.number_value)) {
                return std::to_string(static_cast<long long>(value.number_value));
            } else {
                return std::to_string(value.number_value);
            }
        case DataType::STRING:
        case DataType::LINK:
        case DataType::PATH:
        case DataType::VARIABLE:
            return escapeXmlString(value.string_value);
        case DataType::BOOLEAN:
            return value.boolean_value ? "true" : "false";
        case DataType::NULL_TYPE:
            return "null";
        case DataType::NOT_A_NUMBER:
            return "NaN";
        case DataType::INFINITE:
            return "Infinity";
        default:
            return "unknown";
    }
}

std::string XmlSerializer::tokensToXml(const std::vector<ParserToken>& tokens) {
    std::stringstream xml;
    xml << "<tokens>\n";
    
    for (const auto& token : tokens) {
        xml << "  <token>\n";
        xml << "    <type>" << escapeXmlString(token.type) << "</type>\n";
        xml << "    <value>" << escapeXmlString(token.value) << "</value>\n";
        xml << "    <start>" << token.start << "</start>\n";
        xml << "  </token>\n";
    }
    
    xml << "</tokens>";
    return xml.str();
}

std::string XmlSerializer::serialize(const ParseResult& result) {
    std::stringstream xml;
    
    #ifdef __EMSCRIPTEN__
    
    if (!result.error.empty()) {
        xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        xml << "<result>\n";
        xml << "  <error>" << escapeXmlString(result.error) << "</error>\n";
        xml << "</result>";
        return xml.str();
    } else {
        std::stringstream json;
        json << "{";
        
        std::stringstream valuesXml;
        valuesXml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        valuesXml << "<justc>\n";
        for (const auto& pair : result.returnValues) {
            valuesXml << "  <" << escapeXmlString(pair.first) << ">";
            valuesXml << valueToXml(pair.second);
            valuesXml << "</" << escapeXmlString(pair.first) << ">\n";
        }
        valuesXml << "</justc>";
        
        json << "\"return\":\"" << escapeXmlString(valuesXml.str()) << "\",";
        
        std::stringstream logsXml;
        logsXml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        logsXml << "<logs>\n";
        for (const auto& log : result.logs) {
            logsXml << "  <log>\n";
            logsXml << "    <type>" << escapeXmlString(log.type) << "</type>\n";
            logsXml << "    <message>" << escapeXmlString(log.message) << "</message>\n";
            logsXml << "    <position>" << log.position << "</position>\n";
            logsXml << "    <time>" << escapeXmlString(log.timestamp) << "</time>\n";
            logsXml << "  </log>\n";
        }
        logsXml << "</logs>";
        
        json << "\"logs\":\"" << escapeXmlString(logsXml.str()) << "\",";
        
        // logfile object
        json << "\"logfile\":{";
        json << "\"file\":\"" << escapeXmlString(result.logFilePath) << "\",";
        json << "\"logs\":\"" << escapeXmlString(result.logFileContent) << "\"";
        json << "}";
        
        json << "}";
        return json.str();
    }

    #else

    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<justc>\n";
    
    for (const auto& pair : result.returnValues) {
        xml << "  <" << escapeXmlString(pair.first) << ">";
        xml << valueToXml(pair.second);
        xml << "</" << escapeXmlString(pair.first) << ">\n";
    }
    
    xml << "</justc>";
    return xml.str();

    #endif
}

std::string XmlSerializer::serialize(const std::vector<ParserToken>& tokens, const std::string& input) {
    std::stringstream xml;
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<parser>\n";
    xml << "  <input>" << escapeXmlString(input) << "</input>\n";
    xml << "  " << tokensToXml(tokens) << "\n";
    xml << "</parser>";
    return xml.str();
}

std::string XmlSerializer::serialize(const std::vector<LogEntry>& logs) {
    std::stringstream xml;
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<logs>\n";
    
    for (const auto& log : logs) {
        xml << "  <log>\n";
        xml << "    <type>" << escapeXmlString(log.type) << "</type>\n";
        xml << "    <message>" << escapeXmlString(log.message) << "</message>\n";
        xml << "    <position>" << log.position << "</position>\n";
        xml << "    <time>" << escapeXmlString(log.timestamp) << "</time>\n";
        xml << "  </log>\n";
    }
    
    xml << "</logs>";
    return xml.str();
}
