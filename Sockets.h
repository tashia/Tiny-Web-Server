/* 
 * File:   Sockets.h
 * Author: shilei
 *
 * Created on March 17, 2012, 8:00 PM
 */

#ifndef SOCKETS_H
#define	SOCKETS_H

#include <string>
#include <netinet/in.h>   //for sockaddr_in
#include <sys/socket.h>

class SocketSystem
{
public:
    SocketSystem();
    ~SocketSystem();
    std::string getHostName();
    std::string getNameFromIp(const std::string &ip);
    std::string getIpFromName(const char* name);
    std::string GetLastMsg(bool WantSocketMsg=true);
    
private:
    static bool loaded;
    static int count;
};

class Socket
{
public:
    Socket();
    Socket(int s);
    Socket &operator=(int sock);
    operator int();
    bool connect(std::string url, int port, bool throwError = false);
    void disconnect();
    bool send(const char* block, size_t len, bool throwError=false);
    bool recv(char* block, bool throwError=false);
    std::string getRemoteIP();
    int getRemotePort();
    std::string getHostName();
    std::string getLocalIP();
    int getLocalPort();
    long getSocketConnectionCount();
private:
    int sockfd_;
    SocketSystem ss_;
    volatile long SocketConnectionCount;
};

inline long Socket::getSocketConnectionCount()
{
    return SocketConnectionCount;
}


////////////////////////////////////////////////////////////
// SocketListener class waits for connections, then delivers
// connected socket

class SocketListener
{
public:
    SocketListener(int port);
    ~SocketListener();
    int waitForConnect();
    void stop();
    std::string getLocalIP();
    int getLocalPort();
    long getInvalidSocketCount();
private:
   // struct sockaddr_storage tcpAddr;
    struct sockaddr_in sin;
    Socket s_;
    SocketSystem ss_;
    volatile long InvalidSocketCount;
};

inline long SocketListener::getInvalidSocketCount()
{
    return InvalidSocketCount;
}
#endif	/* SOCKETS_H */

