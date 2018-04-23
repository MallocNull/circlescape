#include "master.hpp"

sosc::MasterIntra::MasterIntra(IntraClient client) {
    this->sock = client;
}

bool sosc::MasterIntra::Process() {
    return true;
}
