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
                return this->Close();

            Packet response;
            if(!this->key.ParseRequest(pck, &response)) {
                this->sock.Send(
                    Packet(EncryptionError, {
                        ""
                    })
                );

                this->Close();
            }

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
