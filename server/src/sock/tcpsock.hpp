#ifndef SOSC_TCPSOCK_H
#define SOSC_TCPSOCK_H

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #ifdef __MINGW32__
    #undef _WIN32_WINNT
    #define _WIN32_WINNT _WIN32_WINNT_WIN8
  #endif 
  #include <winsock2.h>
  #include <ws2tcpip.h>
  
  #define SOSC_SOCK_T SOCKET
  #define SOSC_ADDR_T SOCKADDR_IN
#else
  #include <arpa/inet.h>
  #include <errno.h>
  #include <fcntl.h>
  #include <netdb.h>
  #include <netinet/in.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <unistd.h>
  
  #define SOSC_SOCK_T int
  #define SOSC_ADDR_T struct sockaddr_in
#endif

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include "../utils/net.hpp"
#include "../utils/string.hpp"

#define SOSC_TCP_BUFLEN 2048

namespace sosc {
class TcpClient {
public:
    TcpClient();
    bool Open(std::string host, std::uint16_t port);
    
    int Recv(std::string* str, bool append = false);
    int Send(const std::string& str);
    
    bool IsDataReady();
    inline bool IsOpen() const {
        // TODO consider changing this
        return this->sock_open;
    }
    
    inline net::IpAddress GetIpAddress() const {
        return this->ip;
    }
    
    void Close();
    ~TcpClient();
private:
    void Open(SOSC_SOCK_T sock, SOSC_ADDR_T addr, int addr_len);
    void SetBlocking(bool will_block);
    
    SOSC_SOCK_T sock;
    bool sock_open;
    
    net::IpAddress ip;
    SOSC_ADDR_T addr;
    int addr_len;
    
    char buffer[SOSC_TCP_BUFLEN];
    
    friend class TcpServer;
};

class TcpServer {
public:
    TcpServer();
    bool Listen(uint16_t port);
    
    int Accept(TcpClient* client);
    
    void Close();
    ~TcpServer();
private:
    SOSC_SOCK_T sock;
    bool sock_open;
};
}

#endif
