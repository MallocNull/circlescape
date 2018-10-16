#ifndef SOSC_INTSOCK_H
#define SOSC_INTSOCK_H

#include "tcpsock.hpp"
#include "sock/packet.hpp"

namespace sosc {
class IntraClient {
public:
    IntraClient();
    bool Open(const std::string& host, uint16_t port, bool secure = false);

    int Receive(Packet* packet, bool block = false);
    bool Send(const Packet& packet);
    
    inline bool IsOpen() const {
        return this->client_open;
    }
    
    inline net::IpAddress GetIpAddress() const {
        return this->client.GetIpAddress();
    }
    
    inline void Close() {
        this->client_open = false;
        this->client.Close();
    }
private:
    void Open(const TcpClient& client);
    
    bool client_open;
    TcpClient client;
    std::string buffer;
    
    friend class IntraServer;
};

class IntraServer {
public:
    IntraServer();
    
    bool Listen(uint16_t port);
    bool Accept(IntraClient* client);
    
    inline bool IsOpen() const {
        return this->server_open;
    }
    
    inline void Close() {
        this->server_open = false;
        this->server.Close();
    }
private:
    bool server_open;
    TcpServer server;
};
}

#endif
