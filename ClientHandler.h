/* 
 * File:   ClientHandler.h
 * Author: shilei
 *
 * Created on May 31, 2012, 7:46 PM
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
  //  std::string getReponseHeaders();
  //  void ProcessRequest();
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

