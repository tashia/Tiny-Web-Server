/* 
 * File:   ClientHandler.h
 * Author: shilei
 *
 * Created on May 31, 2012, 7:46 PM
 */
/*        Module Description
 *======================================
 * This ClientHandler class is a derived class of
 * ThreadBase, it is constructed by taking a socket.
 * It provides the public method getRequestHeaders(),
 * so client is able to track the httprequest. However,
 * it will not disclose any content to the client. 
 * Private scope contains several helper functions to
 * parse the request step by step.
 *
 *
 *
 */
#ifndef CLIENTHANDLER_H
#define	CLIENTHANDLER_H

#include "Socket.h"
#include "LThread.h"
#include "IRequestHandler.h"
#include "HandlerFactory.h"

#define MaxSize 1024

class ClientHandler : public ThreadBase
{
public:
    ClientHandler(Socket& sock);
    std::string getRequestHeaders();
    virtual ~ClientHandler();
private:
    void ReadHeaders();
    void ParseHeaders();
    void run();
    bool _hasParsed;
    std::string _requestHeaders;
    /*allocate inside delete outside*/
    void getRequestMessagebody(char** msgBuf);
    Socket _sock;
    Http_Request* _httpRequest;
    IRequestHandler *pIRH;
};


#endif	/* CLIENTHANDLER_H */

