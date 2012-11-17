
#include "ClientHandler.h"
#include <stdio.h>
#include "RequestParser.h"
#include "HandlerFactory.h"

//--------------<ctor>-------------------------------

ClientHandler::ClientHandler(Socket& sock) : _sock(sock) {
    _httpRequest = new Http_Request();
    if (_httpRequest == 0) {
        /* handle the lack of memory */
    }
}


//--------------< destructor >-----------------------

ClientHandler::~ClientHandler() {
    if(_httpRequest->_requestContent != 0)
        delete [] _httpRequest->_requestContent;
    delete _httpRequest;
}

//--------------< read headers of request >----------

void ClientHandler::ReadHeaders() { //\n\r\n is the terminate mark, read char by char
    char endMark[4] = "\n\r\n";
    int matchCount = 0;
    char readBuf[1];
    while (matchCount != 3) {
        if (_sock.Receive(readBuf, 1) != 1) {
            perror("ClientHandler: receive");
            break;
        }
        _requestHeaders += readBuf[0];
        if (readBuf[0] == endMark[matchCount])
            matchCount++;
        else
            matchCount = 0;
    }
    //see the request
 //   std::cout<<_requestHeaders<<std::endl;
}

//--------------< get request Headers >--------------
std::string ClientHandler::getRequestHeaders()
{
    if(!_hasParsed)
    {
        run();
    }
     return _requestHeaders;   
}

//-------------< parse the request >-----------------

void ClientHandler::ParseHeaders()
{
    RequestParser rp(_requestHeaders);
    _httpRequest->_method = rp.getRequestMethod();
    _httpRequest->_relativeURL = rp.getRelativeURL();
    _httpRequest->_protocal = rp.getRequestProtocal();
    _httpRequest->_host = rp.getHost();
    _httpRequest->_connectionStatus = rp.getConnectionStatus();
    // here to check the connection Status
   /* if (_httpRequest->_connectionStatus.compare("keep-alive"))
        _sock.Close();*/
    
    _httpRequest->_content_Length = rp.getContentLength();
    _httpRequest->_encoding = rp.getAcceptEncoding();
    _httpRequest->_content_Type = rp.getContentType();
    //_requestHeaders=rp.TrimedHeaders();
    getRequestMessagebody(&_httpRequest->_requestContent);
}

//--------------< run function >---------------------

void ClientHandler::run() {
    ReadHeaders();
    ParseHeaders();
    pIRH = RequestHandlerFactory::CreateRequestHandler(*_httpRequest);
    std::string headers = pIRH->getResponseHeaders();
    
    _sock.Send(headers.c_str(),headers.size());
    int msgBodyLen = pIRH->getResponseLength();
    if(msgBodyLen > 0)
    {
        const char* msgBody;
        pIRH->getResponseBody(&msgBody);
        _sock.Send(msgBody,msgBodyLen);
    }
    _sock.Close(); 
    
}


//----------------< get message body >--------------------------
void ClientHandler::getRequestMessagebody(char** msgBuf)
{
    if (_httpRequest->_content_Length > 0)
    {
        if (*msgBuf != 0)
            delete [] *msgBuf;
        *msgBuf = new char[_httpRequest->_content_Length];
        // should check how many bytes you have received 
        if (_sock.Receive(*msgBuf,_httpRequest->_content_Length)==-1)
            perror("receive message body error: ");
        return;
    }
    *msgBuf=0;   
}


//--------------< test stub >-------------------------------

#ifdef TEST_CLIENTHANDLER

#include<iostream>

int main() {
    ServerSocket server;
    server.Bind(3491);
    Socket client;
    client.Connect("localhost", 3491);
    Socket comSock = server.Accept();
    std::string headers = "eirwleGETGET /index.htm HTTP/1.1\n"
                          "Accept-Language: en-us,en;q=0.5\r\n"
                          "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686;" 
                          " rv:12.0) Gecko/20100101 Firefox/12.0\n"
                          "Host: localhost:3490\n"
                          "Content-Length: 115\r\n"
                          "Connection: keep-alive\r\n"
                           "\r\n noise!!!!!!";
    int s = headers.size();
    std::cout << "send size :" << s << std::endl;
    client.Send(headers.c_str(), s);
    std::cout << "actual send size : " << s << std::endl;

   // ClientHandler ch(comSock);
  //  std::string rheaders = ch.ReadHeaders();
  //  std::cout << headers << std::endl;
   // std::cout << "recev size is " << headers.size() << std::endl;
    RequestParser rp(headers);
  //  std::cout<<"\n"<<rp.getConnectionStatus();
    std::cout<<"\n"<<rp.getRelativeURL();
    std::cout<<"\n"<<rp.getRequestMethod();
    std::cout<<"\n"<<rp.getRequestProtocal();
    std::cout<<"\nlength:"<<rp.getContentLength()<<"?";
    std::cout<<"\n"<<rp.getConnectionStatus();
    //headers = rp.getConnectionStatus();
    std::cout<<"\n"<<rp.TrimedHeaders();
    if (headers.find("keep-alive")==0)
        std::cout<<"\n cr has been removed"<<std::endl;
}


#endif
