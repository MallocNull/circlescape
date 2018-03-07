#include "tcpsock.hpp"
#ifndef _WIN32

/****************************/
/*   BEGIN TCPCLIENT CODE   */
/****************************/

sosc::TcpClient::TcpClient() {
    this->sock_open = false;
    this->addr_len = -1;
}

bool sosc::TcpClient::Init(std::string host, std::uint16_t port) {
    if(this->sock_open)
        return false;
    
    struct addrinfo hints, *result, *ptr;
    return true;
}

/****************************/
/*    END TCPCLIENT CODE    */
/****************************/
/*   BEGIN TCPSERVER CODE   */
/****************************/

sosc::TcpServer::TcpServer() {
    
}

/****************************/
/*    END TCPSERVER CODE    */
/****************************/

#endif
