
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
        std::cout << "remote ip: " << sock.getRemoteIP() << std::endl;
        std::cout << "remote port: " << sock.getRemotePort() << std::endl;
        pCH->start();
    } while (true);
}



//----------------< test stub >----------------------

#ifdef TEST_TINYWEBSERVER

#include <iostream>

void getTest(char* t) {
    char* m = new char[7];
    for (int i = 0; i < 6; i++)
        m[i] = 'a';
    m[6] = '\0';
    t = m;
    std::cout << t << std::endl;
}

int main() {
    HttpServer hs(3490);
    hs.BeginService();

}
#endif