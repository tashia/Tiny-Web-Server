
#include "Socket.h"
#include <string.h>   //memset
#include <stdio.h>    //sscanf stderr fprintf  perror 
#include <stdlib.h>   // exit
#include <unistd.h>   // close
#include <arpa/inet.h> // inet_ntop, inet_pton
#include <iostream>

/* ServerSocket Section*/
//---------------< constructor >----------------------

ServerSocket::ServerSocket() {
}

//---------------< bind >-----------------------

void ServerSocket::Bind(int port, const char* addr, int backlog) {
    int yes = 1;
    struct addrinfo *p;
    char portChar[7];
    sprintf(portChar, "%d", port);
    _sa.Init(addr, portChar);
    //loop through all the results and bind to the first we can
    for (p = _sa.addrList; p != NULL; p = p->ai_next) {
        if ((_sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        /* to reuse the port*/
        if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof (int)) == -1) {
            perror("setsockpot");
            exit(1);
        }
        // Sock descriptor, address, size of address 
        // kernel will assign a random port to the socket
        if (bind(_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(_sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        return;
    }
    freeaddrinfo(_sa.addrList); // free the linked-list 
    if (listen(_sockfd, backlog) == -1) {
        perror("listen");
        exit(1);
    }
}


//---------------< accept method >---------------------

Socket ServerSocket::Accept() {
    socklen_t addr_size = sizeof _peer_addr;
    int new_fd;
    do {
        new_fd = accept(_sockfd, (struct sockaddr *) &_peer_addr,
                &addr_size);
    } while (new_fd == -1);
    Socket s(new_fd);
    return s; //need copy or assign constructor
}

//---------------< close server socket >----------------

void ServerSocket::Close() {
    close(_sockfd);
}

//---------------< get local port >---------------------

int ServerSocket::getLocalPort() {
    struct sockaddr_storage _local_addr;
    socklen_t slen = sizeof _local_addr;
    int getsock_check = getsockname(_sockfd, (struct sockaddr *) &_local_addr, &slen);
    if (getsock_check == -1) {
        perror("getsockname:");
        exit(1);
    }
    return _sa.getPort((struct sockaddr *) &_local_addr);
}

//---------------< get local ip >-----------------------

std::string ServerSocket::getLocalIP() {
    struct sockaddr_storage _local_addr;
    socklen_t slen = sizeof _local_addr;
    int getsock_check = getsockname(_sockfd, (struct sockaddr *) &_local_addr, &slen);
    if (getsock_check == -1) {
        perror("getsockname:");
        exit(1);
    }
    return _sa.getAddr((struct sockaddr *) &_local_addr);
}


/* Socket Section*/

//---------------< constructor >----------------------

Socket::Socket() {
}



//---------------< promotion constructor >------------

Socket::Socket(int sockfd) {
    _sockfd = sockfd;
}

//---------------< copy constructor >-----------------

Socket::Socket(const Socket& sock) {
    _sockfd = sock._sockfd;
}

//---------------< assignment operator >--------------

Socket& Socket::operator =(const Socket& rhs) {
    if (this != &rhs)
        this->_sockfd = rhs._sockfd;
    return *this;
}

//---------------< destructor >------------------------

Socket::~Socket() {

}

//---------------< cast operator >---------------------

Socket::operator int() {
    return _sockfd;
}

//---------------< connect to remote point >-----------

void Socket::Connect(const char* connectAddr, int port) {
    struct addrinfo *p;
    char portChar[7];
    sprintf(portChar, "%d", port);
    _sa.Init(connectAddr, portChar);
    for (p = _sa.addrList; p != NULL; p = p->ai_next) {
        if ((_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)
                ) == -1) {
            perror("client:socket");
            continue;
        }
        if (connect(_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(_sockfd);
            perror("client: connect");
            continue;
        }
        //  _peer_addr = *(p->ai_addr); /*get info who your connecting to*/
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return;
    }
    freeaddrinfo(_sa.addrList);
}


//---------------< receive data in socket buffer up to size len >-----

int Socket::Receive(char* recevStream, int len, int flags, int recevSize) {
    if (len < recevSize)
        recevSize = len;
    int total_recevd = 0;
    int bytes_left = len;
    int bytes_recevd;
    do {
        if (bytes_left < recevSize)
            recevSize = bytes_left;
        bytes_recevd = recv(_sockfd, recevStream + total_recevd, recevSize/**/, flags);
        if (bytes_recevd == -1)
            break;
        total_recevd += bytes_recevd;
        bytes_left -= bytes_recevd;
    } while (bytes_recevd == recevSize && total_recevd < len);
    return total_recevd;
}


//--------------< send data via socket >---------------

int Socket::Send(const char* msg, int len, int flags, int sendSize) {
    if (len < sendSize)
        sendSize = len;
    int total = 0;
    int bytes_sent, bytes_left = len;
    do {
        if (bytes_left < sendSize)
            sendSize = bytes_left;
        bytes_sent = send(_sockfd, (void*) (msg + total), sendSize, flags);
        if (bytes_sent <= 0) {
            break;
        }
        total += bytes_sent;
        bytes_left -= bytes_sent;
    } while (bytes_sent == sendSize && total < len);
    return total;
}
//--------------< close the socket >--------------------

void Socket::Close() {
    close(_sockfd);
}

//--------------< get remote ip >-----------------------

std::string Socket::getRemoteIP() {
    // getpeername
    socklen_t len;
    len = sizeof _peer_addr;
    getpeername(_sockfd, (struct sockaddr*) &_peer_addr, &len);
    return _sa.getAddr((struct sockaddr*) &_peer_addr);
}

//--------------< get remote port >----------------------

int Socket::getRemotePort() {
    socklen_t len;
    len = sizeof _peer_addr;
    getpeername(_sockfd, (struct sockaddr*) &_peer_addr, &len);
    return _sa.getPort((struct sockaddr*) &_peer_addr);
}

int Socket::getLocalPort() {
    struct sockaddr_storage _local_addr;
    socklen_t slen = sizeof _local_addr;
    int getsock_check = getsockname(_sockfd, (struct sockaddr *) &_local_addr, &slen);
    if (getsock_check == -1) {
        perror("getsockname:");
        exit(1);
    }
    return _sa.getPort((struct sockaddr *) &_local_addr);
}

std::string Socket::getLocalIP() {
    struct sockaddr_storage _local_addr;
    socklen_t slen = sizeof _local_addr;
    int getsock_check = getsockname(_sockfd, (struct sockaddr *) &_local_addr, &slen);
    if (getsock_check == -1) {
        perror("getsockname:");
        exit(1);
    }
    return _sa.getAddr((struct sockaddr *) &_local_addr);
}
/* Socket Assistant */

//-------------------------------------------------------
// specify if it is IPv4 or IPv6

void *SocketAssistant::getInetAddr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET6) {
        return &(((struct sockaddr_in6*) sa)->sin6_addr);
    }
    return &(((struct sockaddr_in*) sa)->sin_addr);
}

//-------------------------------------------------------
// get the port 

int SocketAssistant::getPort(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET6) {
        return ntohs(((struct sockaddr_in6*) sa)->sin6_port);
    }
    return ntohs(((struct sockaddr_in*) sa)->sin_port);
}

//--------------------------------------------------------
// get the ip address

std::string SocketAssistant::getAddr(struct sockaddr* sa) {
    char str[INET6_ADDRSTRLEN];
    inet_ntop(sa->sa_family, getInetAddr(sa), str, INET6_ADDRSTRLEN);
    return str;
}

//----------------------------------------------------------
// get the name info

std::string SocketAssistant::getName(struct sockaddr* sa) {
    char host[1024];
    char service[20];
    getnameinfo(sa, sizeof (*sa), host, 1024, service, 20, NI_NOFQDN);
    return host;
}

//-----------------------------------------------------------
// get address by name

std::string SocketAssistant::getAddrByName(const std::string& hostname) {
    Init(hostname.c_str());
    return getAddr(addrList->ai_addr);
}

//-------------------------------------------------------------
// get name by address

std::string SocketAssistant::getNameByAddr(const std::string& addr) {
    Init(addr.c_str());
    return getName(addrList->ai_addr);
}

//--------------------------------------------------------------
// initialization

void SocketAssistant::Init(const char* nodename, const char* sevname) {
    int status;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    //   hints.ai_flags = AI_PASSIVE; // fill in my IP for me
    if ((status = getaddrinfo(nodename, sevname, &hints, &addrList)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
}


//--------------< test stub >--------------------------

#ifdef TEST_SSOCKET
#include <fstream>

int ResourceRetrieve(const std::string& URI, char** memblock) {
    std::ifstream::pos_type size;
    /* ios::ate set initial position at the end of the file, so tellg() give the size of the file*/
    std::ifstream resource(URI.substr(1).c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (resource.is_open()) {
        size = resource.tellg(); /* get current read position*/
        *memblock = new char[size];
        resource.seekg(0, std::ios::beg);
        resource.read(*memblock, size);
        resource.close();
        return size;
    } else {
        return size = 0;
    }

}

using namespace std;

int main() {
    /*test of socket assistant*/
    SocketAssistant sa;
    string addr = sa.getAddrByName("www.syr.edu");
    cout << "address is " << addr << endl;
    string hostname = sa.getNameByAddr(addr);
    cout << "host name is " << hostname << endl;
    addr = sa.getAddrByName(hostname);
    cout << "address is " << addr << endl;


    /*test of socket and server socket*/

    Socket client;
    ServerSocket server;
    server.Bind(3491, "127.0.0.1");
    client.Connect("127.0.0.1", 3491);
    Socket commuSock = server.Accept();
    /*client communicates with commuSock*/
    string sendMsg = "msg sent";
    string respMsg = "msg recevd";
    string sigClient = "from client";
    string sigServer = "from server";
    char rbuf[100];
    int sentBytes = client.Send((sendMsg + " " + sigClient).c_str(), 7);
    cout << "sent bytes is " << sentBytes << endl;
    int recvBytes; 
    sentBytes = client.Send((sendMsg + " " + sigClient).c_str(), 50, 0, 30);
    cout << "sent bytes is " << sentBytes << endl;
    recvBytes = commuSock.Receive(rbuf, 50, 0, 30);
    rbuf[recvBytes] = '\0';
    cout << "recevd bytes is " << recvBytes << "\n message is "
            << rbuf << endl;
    cout << "client connecting to " << client.getRemoteIP() << " and the port is "
            << client.getRemotePort() << endl;
    cout  << "  the local port is "
            << server.getLocalPort() << " local ip is " << server.getLocalIP() << endl;

    client.Close();
    commuSock.Close();
    
    
    client.Connect("127.0.0.1", 3491);
    commuSock = server.Accept();
    commuSock.Send((respMsg + " " + sigServer).c_str(), 50);
    client.Receive(rbuf, 50);
    cout << "\n message is " << rbuf << endl;

    cout << "client connecting to " << client.getRemoteIP() << " and the port is "
            << client.getRemotePort() << endl;
    cout << " and the local port is "
            << server.getLocalPort() << " local ip is " << server.getLocalIP() << endl;
    client.Close();
    commuSock.Close();

    client.Connect("128.230.171.184", 80);
    cout << client.getRemoteIP() << endl;
    cout << client.getRemotePort() << endl;
    client.Close();
    cout<<"open the browse type 127.0.0.1:3491 to call the socket\n";
    Socket ns = server.Accept();
    cout << ns.getRemoteIP() << endl;
    cout << ns.getRemotePort() << endl;
    cout<<"server ip address is : "<<server.getLocalIP()<<"\n";
    cout<<"server monitori port is : "<<server.getLocalPort()<<"\n";
    /*   commuSock = server.Accept();
         char webSockBuf[1024];
         int recSize = commuSock.Receive(webSockBuf,1023);
         webSockBuf[recSize] = '\0';  
         cout<<"receive size is "<<recSize<<" "<<webSockBuf<<endl;*/
    /* For the purpose of testing post method of tiny web server*/
    /*
       Socket postSock;
       char* msgBody;
       int size = ResourceRetrieve("/3rd_front.jpg",&msgBody);
       std::cout<<size<<endl;
       postSock.Connect("localhost",3490);
       std::string postHeader= "POST /Post/post.jpg HTTP/1.1\r\n"
       "Host: localhost:3490\r\n"
       "Accept-Encoding: gzip, deflate\r\n"
       "Content-Type: image/jpeg\r\n"
       "Content-Length: 21292\r\n\r\n";
       postSock.Send(postHeader.c_str(),postHeader.size());
       postSock.Send(msgBody,size);
     */

    return 0;
}
#endif
