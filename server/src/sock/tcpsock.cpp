#include "tcpsock.hpp"

/****************************/
/*   BEGIN TCPCLIENT CODE   */
/****************************/

#ifdef _WIN32
sosc::TcpClient::TcpClient() {
    this->sock_open = false;
    this->addr_len = -1;
}

bool sosc::TcpClient::Init(std::string host, std::uint16_t port) {
    if(this->sock_open)
        return false;
    
    struct addrinfo hints, *result, *ptr;
    
}

#else

#endif

/****************************/
/*    END TCPCLIENT CODE    */
/****************************/
/*   BEGIN TCPSERVER CODE   */
/****************************/

#ifdef _WIN32
sosc::TcpServer::TcpServer() {
    
}
#else

#endif

/****************************/
/*    END TCPSERVER CODE    */
/****************************/
