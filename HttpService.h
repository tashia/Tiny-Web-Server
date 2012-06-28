/* 
 * File:   HttpService.h
 * Author: shilei
 *
 * Created on April 16, 2012, 12:59 PM
 */

#ifndef HTTPSERVICE_H
#define	HTTPSERVICE_H

#include <string>
#include "Sockets.h"
#include "LThread.h"
#include "IRequestHandler.h"

#define MaxSize 1024


class HttpServer
{
public:
    HttpServer(int port);
    ~HttpServer();
    void BeginService();
private:
    SocketListener s_;
            
};

class ClientHandler  : public ThreadBase
{
public:
    ClientHandler(int* sock);
    ~ClientHandler(){}
    void GetRequestInfo(const std::string& request);
private:
    void ParseHeaders(const std::string& requestHeader);
    void ParseRelativeURL(const std::string& url);
    void run();
    int* _sock;
    Http_Request _httpRequest;
};


//-------------<parse http request>-------------------------
class RequestParser
{
public:
    RequestParser(const std::string& request) : _request(request){}
    ~RequestParser(){}
    std::string getRequestMethod();
    std::string getRequestProtocal();
//    std::string getContentType();
    std::string getRelativeURL();
    std::string getMessageBody();       //later change to pointer 
    int getContentLength();
    int StringToInt(std::string& str);
private:
   const std::string& _request;       //must be const
};
#endif	/* HTTPSERVICE_H */

