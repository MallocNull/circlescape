#include "master.hpp"

sosc::MasterIntra::MasterIntra(const IntraClient& client) {
    this->sock = client;
}

bool sosc::MasterIntra::Process() {
    Packet pck;
    int status = this->sock.Receive(&pck);
    if(status == PCK_ERR)
        return this->Close();
    else if(status == PCK_MORE)
        return true;

    switch(pck.GetId()) {
        case kInitAttempt:
            return this->InitAttempt(pck);
        case kAuthentication:
            return this->Authentication(pck);
        case kStatusUpdate:
            return this->StatusUpdate(pck);
        default:
            return this->Close();
    }
}

bool sosc::MasterIntra::InitAttempt(sosc::Packet &pck) {
    if(!pck.Check(1, key.key_size_bytes))
        return this->Close(Packet(kEncryptionError, { "\x01" }));

    Packet response;
    if(!this->key.ParseRequest(pck, &response, kKeyExchange))
        return this->Close(Packet(kEncryptionError, { "\x02" }));

    this->sock.Send(response);
}

bool sosc::MasterIntra::Authentication(sosc::Packet &pck) {

}

bool sosc::MasterIntra::StatusUpdate(sosc::Packet &pck) {

}

bool sosc::MasterIntra::Close() {
    this->sock.Close();
    return false;
}

bool sosc::MasterIntra::Close(const Packet &message) {
    this->sock.Send(message);
    this->Close();
}
