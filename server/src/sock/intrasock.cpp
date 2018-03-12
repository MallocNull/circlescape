#include "intrasock.hpp"

/******************************/
/* BEGIN INTRACONNECTION CODE */
/******************************/

sosc::IntraConnection::IntraConnection(TcpClient client) {
    this->client = client;
    this->client_open = true;
}

int sosc::IntraConnection::Receive(Packet* packet, bool block) {
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

bool sosc::IntraConnection::Send(const Packet& packet) {
    if(!this->client_open)
        return false;
    
    
}

/****************************/
/* END INTRACONNECTION CODE */
/****************************/
/*  BEGIN INTRASERVER CODE  */
/****************************/

/****************************/
/*   END INTRASERVER CODE   */
/****************************/
