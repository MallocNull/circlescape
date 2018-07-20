#include "tcpsock.hpp"
#ifndef _WIN32

/****************************/
/*   BEGIN TCPCLIENT CODE   */
/****************************/

sosc::TcpClient::TcpClient() {
    this->sock_open = false;
    this->addr_len = -1;
}

bool sosc::TcpClient::Open(std::string host, std::uint16_t port) {
    if(this->sock_open)
        return false;
    
    struct addrinfo hints, *results, *ptr;
    bzero((char*)&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    if(getaddrinfo(host.c_str(), TOSTR(port).c_str(), &hints, &results) != 0)
        return false;
    
    for(ptr = results; ptr != NULL; ptr = ptr->ai_next) {
        this->sock = socket(ptr->ai_family, 
            ptr->ai_socktype, ptr->ai_protocol);
        
        if(this->sock < 0) {
            freeaddrinfo(results);
            return false;
        }
        
        if(connect(this->sock, ptr->ai_addr, (int)ptr->ai_addrlen) == 0)
            break;
        
        close(this->sock);
        this->sock = -1;
    }
    
    freeaddrinfo(results);
    if(this->sock < 0)
        return false;
    
    this->sock_open = true;
    return true;
}

void sosc::TcpClient::Open
    (SOSC_SOCK_T sock, SOSC_ADDR_T addr, int addr_len)
{
    if(this->sock_open)
        return;
    
    this->sock = sock;
    this->sock_open = true;
    
    this->addr = addr;
    this->addr_len = addr_len;
    
    char buffer[128];
    inet_ntop(addr.sin_family, (void*)&addr.sin_addr, buffer, 128);
    this->ip.Parse(buffer);
}

int sosc::TcpClient::Receive(std::string* str, int flags) {
    if(!this->sock_open)
        return -1;
    
    bool append = (flags & SOSC_TCP_APPEND) != 0,
         block  = (flags & SOSC_TCP_BLOCK) != 0;
         
    int total_length = 0;
    bool first_recv = true;
    while(block ? (first_recv ? true : this->IsDataReady()) 
                : this->IsDataReady()) 
    {
        int length = recv(this->sock, this->buffer, SOSC_TCP_BUFLEN, 0);
        if(length <= 0) {
            this->Close();
            return -1;
        }
        
        if(first_recv && !append)
            *str = std::string(this->buffer, length);
        else
            *str += std::string(this->buffer, length);
        
        total_length += length;
        first_recv = false;
    }
    
    return total_length;
}

bool sosc::TcpClient::Send(const std::string& str) {
    if(!this->sock_open)
        return false;
    
    std::string::size_type total_sent = 0;
    while(total_sent < str.length()) {
        int sent = total_sent == 0 
            ? send(this->sock, str.c_str(), str.length(), 0)
            : send(this->sock, str.substr(total_sent).c_str(), 
                   str.length() - total_sent, 0);
        
        if(sent == -1) {
            this->Close();
            return false;
        } else
            total_sent += sent;
    }
    
    return true;
}

bool sosc::TcpClient::IsDataReady() {
    if(!this->sock_open)
        return false;
    
    this->SetBlocking(false);
    int check = recv(this->sock, this->buffer, 1, MSG_PEEK),
        error = errno;
    this->SetBlocking(true);
    
    if(check <= 0) {
        if(check != 0 && (error == EWOULDBLOCK || error == EAGAIN))
            return false;
        else {
            this->Close();
            return false;
        }
    } else
        return true;
}

void sosc::TcpClient::SetBlocking(bool will_block) {
    if(!this->sock_open)
        return;
    
    int flags = fcntl(this->sock, F_GETFL, 0);
    flags = will_block ? flags & ~O_NONBLOCK
                       : flags |  O_NONBLOCK;
    fcntl(this->sock, F_SETFL, flags);
}

void sosc::TcpClient::Close() {
    if(!this->sock_open)
        return;
    
    this->sock_open = false;
    shutdown(this->sock, SHUT_RDWR);
    close(this->sock);
}

/****************************/
/*    END TCPCLIENT CODE    */
/****************************/
/*   BEGIN TCPSERVER CODE   */
/****************************/

sosc::TcpServer::TcpServer() {
    this->sock_open = false;
}

bool sosc::TcpServer::Listen(uint16_t port) {
    if(this->sock_open)
        return false;
    
    struct addrinfo hints, *result;
    bzero((char*)&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    
    if(getaddrinfo(NULL, TOSTR(port).c_str(), &hints, &result) != 0)
        return false;
    
    this->sock = socket(result->ai_family, 
        result->ai_socktype, result->ai_protocol);
                        
    if(this->sock < 0) {
        freeaddrinfo(result);
        return false;
    }
    
    if(bind(this->sock, result->ai_addr, (int)result->ai_addrlen) < 0) {
        freeaddrinfo(result);
        close(this->sock);
        return false;
    }
    
    freeaddrinfo(result);
    if(listen(this->sock, SOMAXCONN) < 0) {
        close(this->sock);
        return false;
    }
    
    this->sock_open = true;
    return true;
}

bool sosc::TcpServer::Accept(TcpClient* client) {
    if(!this->sock_open)
        return false;
    
    SOSC_SOCK_T sock;
    SOSC_ADDR_T addr;
    unsigned int addr_len = sizeof(addr);
    
    sock = accept(this->sock, (struct sockaddr*)&addr, &addr_len);
    if(sock < 0) {
        this->Close();
        return false;
    }
    
    client->Close();
    client->Open(sock, addr, addr_len);
    return true;
}

void sosc::TcpServer::Close() {
    if(!this->sock_open)
        return;
    
    this->sock_open = false;
    shutdown(this->sock, SHUT_RDWR);
    close(this->sock);
}

/****************************/
/*    END TCPSERVER CODE    */
/****************************/

#endif
