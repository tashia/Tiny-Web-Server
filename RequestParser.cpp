#include "RequestParser.h"
#include <sstream>
#include <iostream>

std::string Methods[] = {"GET ", "POST ", "PUT ", "DELETE ", "HEAD ", ""};

//--------------< constructor >----------------------------------

RequestParser::RequestParser(std::string& request) : _request(request) {
    for (int i = 0; !Methods[i].empty(); i++) {
        _startPos = _request.find(Methods[i]);
        if (_startPos != std::string::npos)
            break;
        _startPos = 0;
    }
}

//--------------< get Http method>----------------------------------------------

std::string RequestParser::getRequestMethod() {
    size_t end;
    end = _request.find(' ', _startPos);
    if (end == std::string::npos || end == 0)
        return "";
    return _request.substr(_startPos, end - _startPos);
}

//--------------< get relative URL>---------------------------------------------

std::string RequestParser::getRelativeURL() {
    size_t begin, end;
    begin = _request.find('/', _startPos);
    if (begin == std::string::npos)
        return "";
    end = _request.find(' ', begin);
    if (begin == std::string::npos)
        return "";

    return _request.substr(begin, end - begin);

}

//--------------< get request protocal>-----------------------------------------

std::string RequestParser::getRequestProtocal() {
    size_t begin, end;
    begin = _request.find("HTTP", _startPos);
    if (begin == std::string::npos)
        return "";
    if ((end = _request.find("\n", begin)) != std::string::npos) {
        std::string p = _request.substr(begin, end - begin);
        if ((end = p.find('\r')) != std::string::npos) { /*delete carriage return*/
            p.erase(end, 1);
        }
        return p;
    } else
        return "";
}

//--------------< get content length >------------------------------------------

int RequestParser::getContentLength() {
    std::string len = getHeaderContent("Content-Length: ");
    if (len.compare("") == 0)
        len = "0";
    return StringToInt(len);
}

//--------------< get content type >---------------------------

std::string RequestParser::getContentType() {
    return getHeaderContent("Content-Type: ");
}

//--------------< get connection status >----------------------

std::string RequestParser::getConnectionStatus() {
    return getHeaderContent("Connection: ");
}

//--------------< get accept encoding >------------------------

std::string RequestParser::getAcceptEncoding() {
    return getHeaderContent("Accept-Encoding: ");
}

//--------------< get host >-----------------------------------

std::string RequestParser::getHost() {
    return getHeaderContent("Host: ");
}

//--------------< return trimed headers >-------------------------

std::string RequestParser::TrimedHeaders() {
    return _request.substr(_startPos);
}

//--------------< string to int >----------------------------------------------

int RequestParser::StringToInt(std::string& str) {
    int val;
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << str;
    ss >> val;
    return val;
}

//--------------< get header content >--------------------------

std::string RequestParser::getHeaderContent(std::string header) {
    int start, end;
    start = _request.find(header, _startPos);
    if (start == std::string::npos)
        return "";
    end = _request.find('\n', start);
    if (end == std::string::npos)
        return "";
    std::string content = _request.substr(start + header.size(), end - start - header.size());
    if ((end = content.find('\r')) != std::string::npos) { /*delete carriage return*/
        content.erase(end, 1);
    }
    return content;
}


