#ifndef SOSC_HOST_MASTER_H
#define SOSC_HOST_MASTER_H

#include "sock/intrasock.hpp"
#include "sock/scapesock.hpp"
#include "sock/pool.hpp"

#include "db/database.hpp"

#include "ctx/master.hpp"

#include <vector>

namespace sosc {
/** MASTER -> CLIENT **/
    
class MasterClient {
public:
    
private:
    ScapeConnection sock;
};

class MasterClientPool : public Pool<MasterClient, ctx::MasterClientContext> {
protected:
    void SetupQueries(Queries* queries) override;
    bool ProcessClient(
        MasterClient& client,
        ctx::MasterClientContext* context,
        const Queries* queries) override
    {
        // TODO implement
        return true;
    }
};

/** MASTER -> SLAVE **/

class MasterIntra {
public:
    explicit MasterIntra(const IntraClient& client);
    bool Process(const Queries* queries);

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

    bool authed;
    int auth_attempts;
    const int MAX_AUTH_ATTEMPTS = 3;

    int32_t server_id;
    std::string license;

    const Queries* queries;
};

class MasterIntraPool : public Pool<MasterIntra, ctx::MasterIntraContext> {
protected:
    void SetupQueries(Queries* queries) override;
    bool ProcessClient
        (MasterIntra& client,
         ctx::MasterIntraContext* context,
         const Queries* queries) override
    {
        return client.Process(queries);
    }
};
}

#endif
