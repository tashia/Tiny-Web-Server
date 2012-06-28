

#include "HttpService.h"
#include "HandlerFactory.h"
#include <iostream>
#include <sstream>
#include <string.h>

HttpServer::HttpServer(int port) : s_(port) {
}

HttpServer::~HttpServer() {
}

void HttpServer::BeginService() {
    do {
        int *psock = new int;
        *psock = s_.waitForConnect();
        ClientHandler* pCH = new ClientHandler(psock);
        pCH->start();
    } while (true);
}


//---------------------<ctor>--------------------------------------------------
ClientHandler::ClientHandler(int* sock) : _sock(sock) {
}

void ClientHandler::run() {
    int byteRecved, sendStatus;
    char buffer[MaxSize + 1];
    std::string requestInfo;
    do {
        byteRecved = ::recv(*_sock, &buffer, MaxSize, 0);
        if (byteRecved == -1) {
            std::cout << "recieved failed\n" << std::endl;
            break;
        }
        buffer[byteRecved] = '\0';
        requestInfo.append(buffer);
    } while (byteRecved == MaxSize);
    std::cout << requestInfo << std::endl;
    // parse that request
    GetRequestInfo(requestInfo);
    RequestHandlerFactory f;
    IRequestHandler* IRH;
    IRH = f.CreateRequestHandler(_httpRequest._request_Type);
    char* response = IRH->Response(_httpRequest);
    unsigned int len = IRH->getResponseLength();
    sendStatus = ::send(*_sock, response, len, 0);
    delete [] response;
    close(*_sock);
}


//------------------< get request information >---------------------------------

void ClientHandler::GetRequestInfo(const std::string& request) {
    RequestParser rp(request);
    _httpRequest._method = rp.getRequestMethod();
    _httpRequest._relativeURL = rp.getRelativeURL();
    _httpRequest._protocal = rp.getRequestProtocal();
    _httpRequest._requestData = rp.getMessageBody();
    _httpRequest._content_Length = rp.getContentLength();
    ParseRelativeURL(_httpRequest._relativeURL);
}

//----------------<parse request header lines>------------------------

void ClientHandler::ParseHeaders(const std::string& requestHeader) {
    size_t end, begin = 0;
    std::string line, subStr;
    end = requestHeader.find("\r\n", begin);
    line = requestHeader.substr(begin, end + 1);
    begin = line.find_first_not_of(' ', 0);
    end = line.find_first_of(' ', begin);
    subStr = line.substr(begin, end - begin);
    //get post
    _httpRequest._method = subStr;
    //relative URL
    begin = end + 1;
    end = line.find(' ', begin);
    subStr = line.substr(begin, end - begin);
    _httpRequest._relativeURL = subStr;
    ParseRelativeURL(subStr);
    //protocal 
    begin = requestHeader.find("HTTP", 0);
    _httpRequest._protocal = requestHeader.substr(begin, 8);
    //find host
    begin = requestHeader.find("Host:", 0);
    end = requestHeader.find("\r\n", begin);
    subStr = requestHeader.substr(begin + 6, end - begin - 6);
    _httpRequest._host = subStr;

}

//--------------<Parse relative URL >------------------------------------------

void ClientHandler::ParseRelativeURL(const std::string& url) {
    if (url.compare("/Port") == 0) {
        _httpRequest._request_Type = PortRequest;
        return;
    }
    if (url.compare("/Process") == 0) {
        _httpRequest._request_Type = ProcRequest;
        return;
    }
    _httpRequest._request_Type = OtherRequest;
    return;
}


//--------------< get Http method>----------------------------------------------

std::string RequestParser::getRequestMethod() {
    size_t end;
    end = _request.find(' ', 0);
    if (end == std::string::npos || end == 0)
        return "";
    return _request.substr(0, end);
}

//--------------< get relative URL>---------------------------------------------

std::string RequestParser::getRelativeURL() {
    size_t begin, end;
    begin = _request.find('/', 0);
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
    begin = _request.find("HTTP", 0);
    if (begin == std::string::npos)
        return "";
    if ((end = _request.find("\r\n", begin)) != std::string::npos)
        return _request.substr(begin, end - begin);
    else if ((end = _request.find("\n", begin)) != std::string::npos)
        return _request.substr(begin, end - begin);
    else
        return "";
}

//--------------< get content length >------------------------------------------

int RequestParser::getContentLength() {
    size_t begin, end;
    begin = _request.find("Content-Length: ", 0);
    if (begin == std::string::npos)
        return 0;
    end = _request.find("\r\n", begin);
    if (end == std::string::npos)
        return 0;
    std::string len = _request.substr(begin + 16, end - begin - 16);
    return StringToInt(len);
}

//--------------< get message body >-------------------------------------------
std::string RequestParser::getMessageBody()
{
    int len = getContentLength();
    if( len == 0)
        return "";
    size_t begin, end;
    begin = _request.find("\r\n\r\n",0);
    if(begin == std::string::npos )
        return "";
    return _request.substr(begin+4,len);
}

//--------------< string to int >----------------------------------------------

int RequestParser::StringToInt(std::string& str) {
    int val;
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << str;
    ss >> val;
    return val;
}

//--------------< test stub >---------------------------------------------------

using namespace std;

int main() {
      HttpServer* pHS = new HttpServer(3490);
      pHS->BeginService();
    std::string hr("GET /index.html HTTP/1.1\r\n");
    hr.append("Host: localhost\r\n");
    hr.append("Content-Length: 12\r\n");
    hr.append("\r\n");
    hr.append("message body");
    RequestParser* pRP = new RequestParser(hr);
    std::cout << pRP->getRequestMethod() << "anyspace" << endl;
    std::cout << pRP->getRelativeURL() << "anyspace" << endl;
    std::cout << pRP->getRequestProtocal() << "anyspace" << endl;
    cout<<pRP->getContentLength()<<"anyspace"<<endl;
    cout<<pRP->getMessageBody()<<"anyspace"<<endl;
    delete pRP;
     delete pHS;
}