#include "intrasock.hpp"

/******************************/
/* BEGIN INTRACONNECTION CODE */
/******************************/

sosc::IntraClient::IntraClient() {
    this->client_open = false;
}

bool sosc::IntraClient::Open
    (const std::string& host, uint16_t port, bool secure)
{
    if(!this->client.Open(host, port, secure))
        return false;
    
    this->client_open = true;
    return true;
}

void sosc::IntraClient::Open(const TcpClient& client) {
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

        std::string::size_type offset = this->buffer.size();
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

    return this->client.Send(packet.ToString());
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

    this->server = TcpServer();
    this->server_open = this->server.Listen(port, true);
    return this->server_open;
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
