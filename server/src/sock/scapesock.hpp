#ifndef SOSC_SCAPESOCK_H
#define SOSC_SCAPESOCK_H

#include "packet.hpp"
#include "tcpsock.hpp"

namespace sosc {
class ScapeConnection {
public:
    
private:
    sosc::TcpClient client;
};

class ScapeServer {
public:
    
private:
    sosc::TcpServer server;
};
}

#endif
