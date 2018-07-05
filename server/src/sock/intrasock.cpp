#include "intrasock.hpp"

/******************************/
/* BEGIN INTRACONNECTION CODE */
/******************************/

sosc::IntraClient::IntraClient() {
    this->client_open = false;
    this->cipher = nullptr;
}

bool sosc::IntraClient::Open(std::string host, uint16_t port) {
    if(!this->client.Open(host, port))
        return false;
    
    this->client_open = true;
    return true;
}

void sosc::IntraClient::Open(TcpClient client) {
    this->client = client;
    this->client_open = true;
}

bool sosc::IntraClient::IsCiphered() const {
    return this->cipher != nullptr;
}

void sosc::IntraClient::SetCipher(cgc::Cipher *cipher) {
    this->cipher = cipher;
    cipher->Parse(&this->buffer);
}

int sosc::IntraClient::Receive(Packet* packet, bool block) {
    if(!this->client_open)
        return PCK_ERR;
    
    int status;
    bool first_recv = true;
    while((status = packet->Parse(this->buffer, &this->buffer)) != PCK_OK) {
        if(status == PCK_ERR)
            return PCK_ERR;
        if(!block && !first_recv)
            return PCK_MORE;

        std::string::size_type offset = this->buffer.size();
        status = this->client.Receive
            (&this->buffer, SOSC_TCP_APPEND | (block ? SOSC_TCP_BLOCK : 0));
        if(this->IsCiphered())
            this->cipher->Parse(&this->buffer, offset);
            
        if(status == -1)
            return PCK_ERR;
        first_recv = false;
    }
    
    return PCK_OK;
}

bool sosc::IntraClient::Send(const Packet& packet) {
    if(!this->client_open)
        return false;
    
    std::string packet_raw;
    packet.ToString(&packet_raw);
    if(this->IsCiphered())
        this->cipher->Parse(&packet_raw);

    return this->client.Send(packet_raw) == 0;
}

/****************************/
/* END INTRACONNECTION CODE */
/****************************/
/*  BEGIN INTRASERVER CODE  */
/****************************/

sosc::IntraServer::IntraServer() {
    this->server_open = false;
}

bool sosc::IntraServer::Listen(uint16_t port) {
    if(this->server_open)
        return false;
    
    return this->server.Listen(port);
}

bool sosc::IntraServer::Accept(IntraClient* client) {
    if(!this->server_open)
        return false;
    
    TcpClient new_client;
    if(this->server.Accept(&new_client) == 0) {
        client->Open(new_client);
        return true;
    } else
        return false;
}

/****************************/
/*   END INTRASERVER CODE   */
/****************************/
