#ifndef SOSC_INTSOCK_H
#define SOSC_INTSOCK_H

#include "tcpsock.hpp"
#include "packet.hpp"

namespace sosc {
class IntraConnection {
public:
    int Receive(Packet* packet, bool block = false);
    bool Send(const Packet& packet);
    
    inline bool IsOpen() const {
        return this->client_open;
    }
    
    void Close();
    ~IntraConnection();
private:
    IntraConnection(TcpClient client);
    
    bool client_open;
    TcpClient client;
    std::string buffer;
    
    friend class IntraServer;
};

class IntraServer {
public:
    bool Listen(uint16_t port);
    int Accept(IntraConnection *client);
    
    inline bool IsOpen() const {
        return this->server_open;
    }
    
    void Close();
    ~IntraServer();
private:
    bool server_open;
    TcpServer server;
};
}

#endif
