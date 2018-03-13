#include "intrasock.hpp"

/******************************/
/* BEGIN INTRACONNECTION CODE */
/******************************/

sosc::IntraClient::IntraClient() {
    this->client_open = false;
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
        
        status = this->client.Receive
            (&this->buffer, SOSC_TCP_APPEND | (block ? SOSC_TCP_BLOCK : 0));
            
        if(status == -1)
            return PCK_ERR;
        first_recv = false;
    }
    
    return PCK_OK;
}

bool sosc::IntraClient::Send(const Packet& packet) {
    if(!this->client_open)
        return false;
    
    return this->client.Send(packet.ToString()) == 0;
}

sosc::IntraClient::~IntraClient() {
    this->Close();
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

int sosc::IntraServer::Accept(IntraClient* client) {
    if(!this->server_open)
        return -1;
    
    TcpClient new_client;
    if(this->server.Accept(&new_client) == 0) {
        client->Open(new_client);
        return 0;
    } else
        return -1;
}

sosc::IntraServer::~IntraServer() {
    this->Close();
}

/****************************/
/*   END INTRASERVER CODE   */
/****************************/
