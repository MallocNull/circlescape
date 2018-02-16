#ifndef SOSC_TCPSOCK_H
#define SOSC_TCPSOCK_H

#ifdef _WIN32
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

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

#define SOSC_TCP_BUFLEN 2048

namespace sosc {
class TcpClient {
public:
    TcpClient();
    bool Init(std::string host, std::uint16_t port);
    
    ~TcpClient();
protected:
    bool Init(SOSC_SOCK_T sock, SOSC_ADDR_T addr, int addr_len);
    
    SOSC_SOCK_T sock;
    bool sock_open;
    
    SOSC_ADDR_T addr;
    int addr_len;
    
    char buffer[SOSC_TCP_BUFLEN];
    
    friend class TcpServer;
};

class TcpServer {
public:
    TcpServer();
    
    ~TcpServer();
protected:
    SOSC_SOCK_T sock;
};
}

#endif
