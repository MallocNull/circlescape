#include "slave.hpp"

sosc::SlaveClient::SlaveClient(const ScapeConnection& client) {
    this->sock = client;
}

