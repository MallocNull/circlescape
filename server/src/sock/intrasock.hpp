#ifndef SOSC_INTSOCK_H
#define SOSC_INTSOCK_H

#include "tcpsock.hpp"
#include "packet.hpp"

namespace sosc {
class IntraClient {
public:
    IntraClient();
    bool Open(std::string host, uint16_t port);
    
    int Receive(Packet* packet, bool block = false);
    bool Send(const Packet& packet);
    
    inline bool IsOpen() const {
        return this->client_open;
    }
    
    inline void Close() {
        this->client_open = false;
        this->client.Close();
    }
    
    ~IntraClient();
private:
    void Open(TcpClient client);
    
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
    
    ~IntraServer();
private:
    bool server_open;
    TcpServer server;
};
}

#endif
