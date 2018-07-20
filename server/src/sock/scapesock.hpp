#ifndef SOSC_SCAPESOCK_H
#define SOSC_SCAPESOCK_H

#include <queue>
#include "../crypto/sha1.hpp"
#include "../crypto/base64.hpp"
#include "../crypto/cipher.hpp"
#include "frame.hpp"
#include "packet.hpp"
#include "tcpsock.hpp"

#define SOSC_SHAKE_ERR (-1)
#define SOSC_SHAKE_CONT 0
#define SOSC_SHAKE_DONE 1

namespace sosc {
class ScapeConnection {
public:
    ScapeConnection();

    bool IsCiphered() const;
    void SetCipher(cgc::Cipher* cipher);

    int Handshake();
    int Receive(Packet* packet, bool block = false);
    bool Send(const Packet& packet);
    
    inline bool IsOpen() const {
        return this->client_open;
    }
    
    inline bool Handshaked() const {
        return this->handshaked;
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
    bool handshaked;
    TcpClient client;
    cgc::Cipher* cipher;
    
    std::string buffer;
    std::string pck_frames;
    
    friend class ScapeServer;
};

class ScapeServer {
public:
    ScapeServer();
    
    bool Listen(uint16_t port);
    bool Accept(ScapeConnection* client);
    
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
