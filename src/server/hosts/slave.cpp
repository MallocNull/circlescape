#include "slave.hpp"

/** SLAVE -> MASTER **/

bool sosc::slave_to_master_thread(IntraClient* client) {
    *client = IntraClient();
    client->Open();
}

/** SLAVE -> CLIENT **/

sosc::SlaveClient::SlaveClient(const ScapeConnection& client) {
    this->sock = client;
}

bool sosc::SlaveClient::Close() {
    this->sock.Close();
    return false;
}

bool sosc::SlaveClient::Close(const Packet& message) {
    this->sock.Send(message);
    return this->Close();
}
