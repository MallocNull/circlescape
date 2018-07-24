#ifndef SOSC_HOST_SLAVE_H
#define SOSC_HOST_SLAVE_H

#include "../sock/scapesock.hpp"
#include "../sock/pool.hpp"

namespace sosc {
/** SLAVE -> CLIENT **/

class SlaveClient {
public:
    explicit SlaveClient(const ScapeConnection& client);
private:
    ScapeConnection sock;
};

class SlaveClientPool : public Pool<SlaveClient> {
protected:
    bool ProcessClient(SlaveClient& client, const Queries* queries) override {
        // TODO implement
        return true;
    }
};
}

#endif
