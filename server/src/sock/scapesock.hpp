#ifndef SOSC_SCAPESOCK_H
#define SOSC_SCAPESOCK_H

#include "packet.hpp"
#include "tcpsock.hpp"

namespace sosc {
class ScapeConnection {
public:
    ScapeConnection();
    
    int Receive(Packet* packet, bool block = false);
    bool Send(const Packet& packet);
    
    inline bool IsOpen() const {
        return this->client_open;
    }
    
    inline void Close() {
        this->client_open = false;
        this->client.Close();
    }
    
    ~ScapeConnection();
private:
    ScapeConnection(TcpClient client);
    
    bool client_open;
    TcpClient client;
    std::string buffer;
    
    friend class ScapeServer;
};

class ScapeServer {
public:
    ScapeServer();
    
    bool Listen(uint16_t port);
    int Accept(ScapeConnection* client);
    
    inline bool IsOpen() const {
        return this->server_open;
    }
    
    inline void Close() {
        this->server_open = false;
        this->server.Close();
    }
    
    ~ScapeServer();
private:
    bool server_open;
    TcpServer server;
};
}

#endif
