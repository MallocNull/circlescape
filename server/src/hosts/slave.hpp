#ifndef SOSC_HOST_SLAVE_H
#define SOSC_HOST_SLAVE_H

#include "../sock/scapesock.hpp"
#include "../sock/pool.hpp"

namespace sosc {
class SlaveClient {
public:
    
private:
    ScapeConnection sock;
};

class SlavePool : public Pool<SlaveClient*> {
protected:
    bool ProcessClient(SlaveClient* client) override;
};
}

#endif
