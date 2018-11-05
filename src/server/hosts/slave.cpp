#include "slave.hpp"

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
