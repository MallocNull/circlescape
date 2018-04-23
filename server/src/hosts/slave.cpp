#include "slave.hpp"

sosc::SlaveClient::SlaveClient(ScapeConnection client) {
    this->sock = client;
}

