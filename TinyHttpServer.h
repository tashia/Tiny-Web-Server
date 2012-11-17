/* 
 * File:   TinyHttpServer.h
 * Author: shilei
 *
 * Created on May 31, 2012, 7:24 PM
 * 
 * Module Description
 * ======================================
 * This package provides a class called HttpServer.
 * The class HttpServer supports to monitor certain port
 * on local host and creates a new thread to do the provide
 * the service. In addition, the server can also send http 
 * request to other server.
 * 
 * 
 * Interface
 * =======================================
 * HttpServer hs(portnumber)      //create an object of HttpServer
 * hs.start()                     // create a new thread to do the service job
 * hs.SendRequest(addr, port, Request);  // send a request to addr:port
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

