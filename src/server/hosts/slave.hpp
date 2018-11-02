#ifndef SOSC_HOST_SLAVE_H
#define SOSC_HOST_SLAVE_H

#include "sock/scapesock.hpp"
#include "sock/pool.hpp"
#include "ctx/slave.hpp"

namespace sosc {
/** SLAVE -> CLIENT **/

class SlaveClient {
public:
    SlaveClient() = delete;
    explicit SlaveClient(const ScapeConnection& client);

    ~SlaveClient();
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
