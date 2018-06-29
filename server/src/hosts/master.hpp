#ifndef SOSC_HOST_MASTER_H
#define SOSC_HOST_MASTER_H

#include "../sock/intrasock.hpp"
#include "../sock/scapesock.hpp"
#include "../sock/pool.hpp"

#include "../crypto/keyex.hpp"
#include "../crypto/cipher.hpp"

#include "../db/database.hpp"

#include <vector>

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
    explicit MasterIntra(const IntraClient& client);
    bool Process(const db::QueryList* queries);

    bool Close();
    bool Close(const Packet& message);
private:
    bool InitAttempt(Packet& pck);
    bool Authentication(Packet& pck);
    bool StatusUpdate(Packet& pck);

    bool AuthenticationFailure
        (const std::string& packetId, uint16_t errorCode);

    bool NotAuthorized(const std::string& packetId);

    enum SlaveToMasterId {
        kInitAttempt = 1,
        kAuthentication,
        kStatusUpdate
    };

    enum MasterToSlaveId {
        kKeyExchange = 1,
        kEncryptionError,
        kPositiveAck,
        kNegativeAck
    };

    IntraClient sock;
    cgc::KeyExchange key;
    cgc::Cipher cipher;

    bool authed;
    int auth_attempts;
    const int MAX_AUTH_ATTEMPTS = 3;
    std::string license;


    const db::QueryList* queries;
};

class MasterIntraPool : public Pool<MasterIntra> {
public:
    MasterIntraPool();
protected:
    bool ProcessClient(MasterIntra& client) override {
        return client.Process(&this->queries);
    }

    void Stop() override;
private:
    db::QueryList queries;
};
}

#endif
