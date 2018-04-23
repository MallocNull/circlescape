#ifndef SOSC_HOST_SLAVE_H
#define SOSC_HOST_SLAVE_H

#include "../sock/scapesock.hpp"
#include "../sock/pool.hpp"

namespace sosc {
class SlaveClient {
public:
    SlaveClient(ScapeConnection client);
private:
    ScapeConnection sock;
};

class SlaveClientPool : Pool<SlaveClient*> {
protected:
    bool ProcessClient(SlaveClient*& client) override {
        // TODO implement
        return true;
    }
};
}

#endif
