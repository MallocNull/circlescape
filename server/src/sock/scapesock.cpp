#include "scapesock.hpp"

/**********************************/
/*   BEGIN SCAPECONNECTION CODE   */
/**********************************/

sosc::ScapeConnection::ScapeConnection() {
    this->client_open = false;
    this->handshaked = false;
}

void sosc::ScapeConnection::Open(TcpClient client) {
    this->client = client;
    this->client_open = true;
}

int sosc::ScapeConnection::Handshake() {    
    if(this->handshaked)
        return SOSC_SHAKE_DONE;
    if(!this->client_open)
        return SOSC_SHAKE_ERR;
    
    if(!this->client.IsDataReady())
        return SOSC_SHAKE_CONT;
    
    this->client.Receive(&this->buffer, SOSC_TCP_APPEND);
    if(!str::contains(this->buffer, "\r\n\r\n"))
        return SOSC_SHAKE_CONT;
    
    if(!str::starts(this->buffer, "GET")) {
        this->Close();
        return SOSC_SHAKE_ERR;
    }
    
    auto lines = str::split(this->buffer, "\r\n");
    
    this->handshaked = true;
    return SOSC_SHAKE_DONE;
}

/******************************/
/*  END SCAPECONNECTION CODE  */
/******************************/
/*   BEGIN SCAPESERVER CODE   */
/******************************/

sosc::ScapeServer::ScapeServer() {
    this->server_open = false;
}

bool sosc::ScapeServer::Listen(uint16_t port) {
    if(this->server_open)
        return false;
    
    this->server = TcpServer();
    this->server.Listen(port);
    this->server_open = true;
    return true;
}

int sosc::ScapeServer::Accept(ScapeConnection* client) {
    TcpClient raw_client;
    int status = this->server.Accept(&raw_client);
    if(status != 0)
        return status;
    
    
    return 0;
}
