/* 
 * File:   Socket.h
 * This Socket class intends to work for both
 * IPv4 and IPv6
 * Author: shilei
 *
 * Created on June 1, 2012, 3:38 PM
 */

 /*
  * This package provides 3 classes : SocketAssistant, Socket and ServerSocket
  * SocketAssistant provides methods to get ip, port, hostname infomation.
  *      : Socket and ServerSocket are two friend classes of this class
  * Socket provides services for the tcp client
  *      : send(), and Receive() methods both support partial send and receive
  * 
  * ServerSocket provides services for tcp server
  * /

/*
 * NOTES: How to tell if it is IPv6 or IPv4
 *        check ss_family AF_INET or AF_INET6
 *        cast sockaddr_storage to sockaddr_in
 *        or sockaddr_in6
 *        SEE BEEJ guide Jumping from IPv4 to IPv6
 * Manipulation of Address:
 *        inet_pton() :numbers-and-dots to network both for IPv4&6
 *                     don't forget to check return value
 *        inet_ntop() : the other way around
 *        
 */
#ifndef SOCKET_H
#define	SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>

class Socket;
class ServerSocket;

class SocketAssistant
{
public:
    std::string getAddrByName(const std::string& hostname);
    std::string getNameByAddr(const std::string& addr);
    friend class Socket;
    friend class ServerSocket;
private:
    void Init(const char* nodename,const char* sevname=NULL);
    std::string getAddr(struct sockaddr* sa);
    std::string getName(struct sockaddr* sa);
    int getPort(struct sockaddr* sa);
    void* getInetAddr(struct sockaddr* sa);
    struct addrinfo hints, *addrList;
};

class Socket
{
public:
    Socket();
    Socket(int sockfd);     //promotion constructor
    Socket(const Socket& ); 
    Socket& operator=(const Socket &rhs);   //assignment 
    operator int();
    ~Socket();
    void Connect(const char* connectAddr, int port);
    int Send(const char *msg, int len, int flags=0, int sendSize=3074); //send up to *len
    int Receive(char* recevStream, int len, int flags=0, int recevSize=3074); //recev up to *len
    void Close(); 
    int getRemotePort();
    std::string getRemoteIP();
private:
    int _sockfd;
    SocketAssistant _sa;
    struct sockaddr_storage _peer_addr;
};


class ServerSocket
{
public:
    ServerSocket();
    void Bind(int port,const char* addr="localhost", int backlog=10);
    Socket Accept();
    void Close();
    int getLocalPort();
    std::string getLocalIP();
    std::string getRemoteIP();
private:
    int _sockfd;
    SocketAssistant _sa;
    struct sockaddr_storage _peer_addr;
    struct addrinfo _local_addrinfo;   
};



#endif	/* SOCKET_H */

