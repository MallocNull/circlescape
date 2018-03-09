#ifndef SOSC_INTSOCK_H
#define SOSC_INTSOCK_H

#include "tcpsock.hpp"

namespace sosc {
class IntraConnection {
public:
    int Receive(Packet* packet, bool block = false);
    bool Send(const Packet& packet);
    
    void Close();
    ~IntraConnection();
private:
    IntraConnection(TcpClient client);
    
    TcpClient client;
    std::string buffer;
    
    friend class IntraServer;
};

class IntraServer {
public:
    
private:
    
};
}

#endif
