#ifndef SOSC_HOST_SLAVE_H
#define SOSC_HOST_SLAVE_H

#include "sock/intrasock.hpp"
#include "sock/scapesock.hpp"
#include "sock/pool.hpp"
#include "ctx/slave.hpp"

namespace sosc {
/** SLAVE -> MASTER **/

bool slave_to_master_thread(IntraClient* client);

/** SLAVE -> CLIENT **/

class SlaveClient {
public:
    SlaveClient() = delete;
    explicit SlaveClient(const ScapeConnection& client);

    bool Close();
    bool Close(const Packet& message);

    ~SlaveClient()  { this->Close(); }
private:
    ScapeConnection sock;
};

class SlaveClientPool : public Pool<SlaveClient, ctx::SlaveContext> {
protected:
    bool ProcessClient
        (SlaveClient* client,
         ctx::SlaveContext* context,
         const db::Queries* queries) override
    {
        // TODO implement
        return true;
    }
};
}

#endif
