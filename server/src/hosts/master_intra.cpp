#include "master.hpp"

sosc::MasterIntra::MasterIntra(IntraClient client) {
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
        case InitAttempt:
            if(!pck.Check(1, key.key_size_bytes))
                return this->Close(Packet(EncryptionError, { "\x01" }));

            Packet response;
            if(!this->key.ParseRequest(pck, &response))
                return this->Close(Packet(EncryptionError, { "\x02" }));

            this->sock.Send(response);
            break;
        case Authentication:

            break;
        default:
            this->Close();
            return false;
    }

    return true;
}

bool sosc::MasterIntra::Close() {
    this->sock.Close();
    return false;
}

bool sosc::MasterIntra::Close(const Packet &message) {
    this->sock.Send(message);
    this->Close();
}
