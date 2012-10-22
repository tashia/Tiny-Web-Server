
#include "TinyHttpServer.h"
#include "ClientHandler.h"

#include <iostream>
//----------------< constructor >-------------------

HttpServer::HttpServer(int port) {
    s_.Bind(port);
}

//----------------< destructor >--------------------

HttpServer::~HttpServer() {
}

//----------------< server start function >---------

void HttpServer::BeginService() {
    do {
        Socket sock = s_.Accept();
        ClientHandler* pCH = new ClientHandler(sock);
        
        pCH->start();
    } while (true);
}

//----------------< send the request >--------------------------------
//
void HttpServer::SendRequest( std::string ipaddr, int port, Request& req) {
  _requestSock.Connect(ipaddr.c_str(), port);
  std::string reqH = req.getRequest();
  _requestSock.Send(reqH.c_str(), reqH.size());
  //send content body
  _requestSock.Send(req.getContentBody(),req.getLength());
}

//----------------< run >--------------------------------
//
void HttpServer::run() {
  BeginService(); 
}

std::string HttpServer::getResponse()
{
  char buf[1024];
  int recvBytes = _requestSock.Receive(buf,1023);
  buf[recvBytes]='\0';
  return buf;
}
//----------------< test stub >----------------------

#ifdef TEST_TINYWEBSERVER

#include <iostream>

using namespace std;
void getTest(char* t) {
    char* m = new char[7];
    for (int i = 0; i < 6; i++)
        m[i] = 'a';
    m[6] = '\0';
    t = m;
    std::cout << t << std::endl;
}

int main() {
    HttpServer hs0(3490);
    HttpServer hs1(3491);
    hs0.start();
  //  hs1.start();
  //  Request req1("POST","/message/","HTTP/1.1");
    Request req0("POST","/message/","HTTP/1.1");
    
  //  req1.addContentBody("hello server1",13);
    req0.addContentBody("hello server0",13);
 //   hs0.SendRequest("127.0.0.1", 3491, req1);
 //   hs1.SendRequest("127.0.0.1", 3490, req0);
    cout<<"finish sending"<<endl;
  //  cout<<hs0.getResponse()<<endl;
    sleep(1);
  //  cout<<"response message:\n";
  //  cout<<hs1.getResponse()<<endl;
    hs0.join();
}
#endif
