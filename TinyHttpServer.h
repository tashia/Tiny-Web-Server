/* 
 * File:   TinyHttpServer.h
 * Author: shilei
 *
 * Created on May 31, 2012, 7:24 PM
 */

#ifndef TINYHTTPSERVER_H
#define	TINYHTTPSERVER_H

#include "Socket.h"

class HttpServer
{
public:
    HttpServer(int port);
    ~HttpServer();
    void BeginService();
private:
    ServerSocket s_;            
};

#endif	/* TINYHTTPSERVER_H */

