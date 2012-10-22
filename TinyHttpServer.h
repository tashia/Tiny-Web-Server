/* 
 * File:   TinyHttpServer.h
 * Author: shilei
 *
 * Created on May 31, 2012, 7:24 PM
 */

#ifndef TINYHTTPSERVER_H
#define	TINYHTTPSERVER_H

#include "Socket.h"
#include "LThread.h"
#include "Request.h"

class HttpServer : public ThreadBase
{
public:
    HttpServer(int port);
    ~HttpServer();
    void BeginService();
    void SendRequest( std::string ipaddr, int port, Request& req);
    // for test purpose
    std::string getResponse();
private:
    void run();
    ServerSocket s_;  
    Socket _requestSock;
};

#endif	/* TINYHTTPSERVER_H */

