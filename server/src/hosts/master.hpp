#ifndef SOSC_HOST_MASTER_H
#define SOSC_HOST_MASTER_H

#include "../sock/intrasock.hpp"
#include "../sock/scapesock.hpp"
#include "../sock/pool.hpp"

#include "../crypto/keyex.hpp"
#include "../crypto/cipher.hpp"

namespace sosc {
/** MASTER -> CLIENT **/
    
class MasterClient {
public:
    
private:
    ScapeConnection sock;
    
    cgc::KeyExchange key;
    cgc::Cipher cipher;
};

class MasterClientPool : public Pool<MasterClient> {
protected:
    bool ProcessClient(MasterClient& client) override {
        // TODO implement
        return true;
    }
};

/** MASTER -> SLAVE **/

class MasterIntra {
public:
    MasterIntra(IntraClient client);
    bool Process();
    bool Close();
private:
    enum kSlaveToMasterId {
        InitAttempt = 1,
        Authentication,
        StatusUpdate
    };

    enum kMasterToSlaveId {
        KeyExchange = 1,
        EncryptionError,
        PositiveAck,
        NegativeAck
    };

    IntraClient sock;
    cgc::KeyExchange key;
    cgc::Cipher cipher;
};

class MasterIntraPool : public Pool<MasterIntra> {
protected:
    bool ProcessClient(MasterIntra& client) override {
        return client.Process();
    }
};
}

#endif
