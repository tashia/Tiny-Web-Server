#include "Sockets.h"
#include <iostream>
#include <errno.h>  //errno
#include <sys/unistd.h> // gethostname
#include <stdio.h> // for perror 
#include <stdlib.h> // for exit
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>  // these three for getaddrinfo()
#include <arpa/inet.h>   // inet_ntop inet_pton
#include <string.h>
#include <pthread.h>

bool SocketSystem::loaded = false;
int SocketSystem::count = 0;

SocketSystem::SocketSystem() {
}

SocketSystem::~SocketSystem() {
}

//------< get host name >---------

std::string SocketSystem::getHostName() {
    int returnVal;
    char buffer[256];
    if (returnVal = gethostname(buffer, sizeof buffer) != -1)
        return buffer;
    perror("get hostname error");
}

//------<get ip address of network machine>---

std::string SocketSystem::getIpFromName(const char* name) {
    // use getaddrinfo() because it supports ipv6 better than gethostbyname()
    struct addrinfo hints, *servinfo;
    int rv;
    char str[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(name, "http", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    struct sockaddr_in* sa = (struct sockaddr_in*) (servinfo->ai_addr);
    freeaddrinfo(servinfo);
    inet_ntop(AF_INET, &(sa->sin_addr), str, INET_ADDRSTRLEN);
    return str;
}

//-----------<get name from ip>----------------

std::string SocketSystem::getNameFromIp(const std::string& ip) {
    struct sockaddr_in sin;
    char host[1024];
    char service[20];
    sin.sin_family = AF_INET; //don't miss this assignment
    //struct in_addr ipv4addr;
    inet_pton(AF_INET, ip.c_str(), &(sin.sin_addr));
    int error = getnameinfo((struct sockaddr*) &sin, sizeof sin, host, sizeof host, service,
            sizeof service, NI_NOFQDN);
    if (error != 0) {
        fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(error));
    }
    // this method add additional string sucks 
    //he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);

    return host;

}

std::string SocketSystem::GetLastMsg(bool WantSocketMsg) {
    if (errno == 0)
        return "no error";
    else
        return "has error";
}

//-----------< Constructor of Socket to create TCP Stream socket >--------------

Socket::Socket() : SocketConnectionCount(0) {
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ == -1)
        perror("invalid socket in constructor");
}

//------------< promotes socket discripter to socket object>------

Socket::Socket(int s) : sockfd_(s), SocketConnectionCount(0) {
}

//----------< assignment >----------------------

Socket &Socket::operator =(int sock) {
    sockfd_ = sock;
    SocketConnectionCount = 0;
    return *this;
}

//----------< return socket descriptor> ------

Socket::operator int() {
    return sockfd_;
}

//---------< connnect >---------------------

bool Socket::connect(std::string url, int port, bool throwError) {
    try {
        int i = 0;
        bool islegalURL = true;
        while (url[i]) {
            if (!isalpha(url[i]) && url[i]!='-') {
                islegalURL = false;
                break;
            }
            i++;
        }
        if (islegalURL)
            url = SocketSystem().getIpFromName(url.c_str()); // new way I didn't know 
    } catch (...) {
        if (throwError)
            //throw std::exception(ss_.GetLastMsg(true).c_str());
            perror("url error");
        return false;
    }
    struct sockaddr_in tcpAddr;
    tcpAddr.sin_family = AF_INET;
    // convert IP addr from a dots-and-number str to a struct in_addr
    //inet_aton(url.c_str(), &(tcpAddr.sin_addr));
    tcpAddr.sin_addr.s_addr = inet_addr(url.c_str());
    tcpAddr.sin_port = htons(port);
    if (sockfd_ == -1) {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    }
    // scope :: is missed before global socop connect
    // cast it in argument sizeof initial variable;
    int crv = ::connect(sockfd_, (struct sockaddr*) &tcpAddr, sizeof tcpAddr);
    if (crv == -1) {
        close(sockfd_);
        perror("connect problem");
        return false;
    }
    __sync_add_and_fetch(&SocketConnectionCount, 1);
    return true;
}

//---------< disconnet socket>---------------------------

void Socket::disconnect() {
    shutdown(sockfd_, SHUT_RDWR); // prevent both sides
    if (close(sockfd_) == -1)
        perror("close error");
    sockfd_ = -1; //for new connection
}

//---------< send blocks until all characters are sent >--

bool Socket::send(const char* block, size_t len, bool throwError) {
    size_t bytesSent; //current number of bytes sent
    size_t blockIndx = 0; // place in buffer to send next
    size_t count = 0; // number of send failures
    const int sendRetries = 100;
    size_t blockLen = len;
    size_t bytesLeft = blockLen;
    while (bytesLeft > 0) {
        bytesSent = ::send(sockfd_, &block[blockIndx], static_cast<int> (bytesLeft), 0);
        if (bytesSent == -1) {
            count++;
            if (count == sendRetries) {
                if (throwError)
                    perror("send failed after 100 retries");
                else
                    return false;
            }
            if (throwError)
                perror("send error");

            sleep(50);
            bytesSent = 0; //Do i have to
        }

        bytesLeft -= bytesSent;
        blockIndx += bytesSent;

    }
    return true;
}

//-------------< blocks until len characters have been sent >------------

bool Socket::recv(char* block, bool throwError) {
    const size_t recvRetries = 100;
    const size_t MaxSize = 10;
    size_t bytesRecvd, blockIndx = 0, count = 0;

    do {
        bytesRecvd = ::recv(sockfd_, &block[blockIndx], static_cast<int> (MaxSize), 0);
        if (bytesRecvd == 0) {
            if (throwError)
                perror("remote connection closed");
            return false;
        }
        if (bytesRecvd == -1) {
            count++;
            if (count == recvRetries) {
                if (throwError)
                    perror("recv failed after 100 retries");
                else
                    return false;
            }
            sleep(50);
            bytesRecvd = 0;
        }
        blockIndx += bytesRecvd;
    } while (bytesRecvd == MaxSize);
    /*
    const size_t recvRetries = 100;
    size_t bytesRecvd, bytesLeft = len;
    size_t blockIndx = 0, count = 0;
    while (bytesLeft > 0) {
        bytesRecvd = ::recv(sockfd_, &block[blockIndx], static_cast<int> (bytesLeft), 0);
        if (bytesRecvd == 0) {
            if (throwError)
                perror("remote connection closed");
            return false;
        }
        if (bytesRecvd == -1) {
            count++;
            if (count == recvRetries) {
                if (throwError)
                    perror("recv failed after 100 retries");
                else
                    return false;
            }
            sleep(50);
            bytesRecvd = 0;
        }
        bytesLeft -= bytesRecvd;
        blockIndx += bytesRecvd;
    }*/
    return true;
}

//------------------<get remote ip address >-------------------

std::string Socket::getRemoteIP() {
    socklen_t len; // socklen is int
    struct sockaddr_storage addr;
    char ipstr[INET6_ADDRSTRLEN];


    len = sizeof addr;
    // return addr info about the remote side of the connection
    if (getpeername(sockfd_, (struct sockaddr*) &addr, &len) == -1) {
        perror("can not get remote ip");
        return "";
    }

    // deal with both IPv4 and IPv6
    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in*) &addr;
        inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6*) &addr;
        inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
    }
    return ipstr;
}

///--------------< get remote port >-------------------------

int Socket::getRemotePort() {
    struct sockaddr_storage addr;
    socklen_t len = sizeof addr;
    int port;

    if (getpeername(sockfd_, (struct sockaddr*) &addr, &len) == -1) {
        perror("fail to get remote port");
        return -1;
    }

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in*) &addr;
        port = ntohs(s->sin_port);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6*) &addr;
        port = ntohs(s->sin6_port);
    }
    return port;
}

//---------------< get local ip >-------------

std::string Socket::getLocalIP() {
    struct addrinfo hints, *servinfo;
    int rv;
    char hostname[128];
    char ipv4str[INET_ADDRSTRLEN];

    gethostname(hostname, sizeof hostname);
    //std::cout<<"local host name is : "<<hostname;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(hostname, NULL, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(rv));
        exit(1);
    }
    inet_ntop(AF_INET, &(((struct sockaddr_in*) servinfo->ai_addr)->sin_addr),
            ipv4str, INET_ADDRSTRLEN);
    return ipv4str;
}

//----------------< get local port>-------------

int Socket::getLocalPort() {
    struct sockaddr_storage addr;
    socklen_t len = sizeof addr;
    if (getsockname(sockfd_, (struct sockaddr*) &addr, &len) == -1) {
        perror("getsockname");
        return -1;
    }
    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in*) &addr;
        return ntohs(s->sin_port);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6*) &addr;
        return ntohs(s->sin6_port);
    }
}

//------------< starts listener socket listening for connections >-------------

SocketListener::SocketListener(int port) : InvalidSocketCount(0) {
    // struct sockaddr_in sin;
    sin.sin_family = AF_INET; // tcp/ip
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY; // listen overstruct sockaddr_in  every network interface
    try {
        if (bind(s_, (struct sockaddr*) &sin, sizeof sin) == -1) {
            perror("binding error");
            throw "binding error";
        }

        int backLog = 10;
        if (listen(s_, backLog) == -1) {
            perror("listen error");
            throw "listen error";
        }
    } catch (const char* e) {
        std::cout << "\n" << e << std::endl;
    }
}

//--------< destructor closes socket >----------------------------

SocketListener::~SocketListener() {
    try {
        shutdown(s_, SHUT_RDWR);
        close(s_);
    } catch (...) {
        /* don't let exception propagate on shutdown*/
    }
}

//---------<blocks until a connection request has been received>----

int SocketListener::waitForConnect() {
    const long MaxCount = 20;
    InvalidSocketCount = 0;
    socklen_t size = sizeof (sin);
    int toClient;
    try {
        do {
            toClient = accept(s_, (struct sockaddr*) &sin, &size);
            __sync_add_and_fetch(&InvalidSocketCount, 1);
            if (InvalidSocketCount >= 20)
                throw "invalid socket connection";
        } while (toClient == -1);
        return toClient;
    } catch (const char* e) {
        std::cout << "\n" << e << std::endl;
    }
}

//----------< shut down listener>----------------------

void SocketListener::stop() {
    shutdown(s_, SHUT_RDWR);
    close(s_);
}

//----------< get local ip address>---------------------

std::string SocketListener::getLocalIP() {
    return s_.getLocalIP();
}

//----------< get local port number>-------------------

int SocketListener::getLocalPort() {
    return s_.getLocalPort();
}
