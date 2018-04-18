#ifndef SOSC_HOST_MASTER_H
#define SOSC_HOST_MASTER_H

#include "../sock/intrasock.hpp"
#include "../sock/scapesock.hpp"
#include "../sock/pool.hpp"

namespace sosc {
/** MASTER -> CLIENT **/
    
class MasterClient {
public:
    
private:
    ScapeConnection client;
};

class MasterClientPool : public Pool<MasterClient> {
protected:
    bool ProcessClient(MasterClient client) override;
};

/** MASTER -> SLAVE **/

class MasterIntra {
public:
    MasterIntra(IntraClient client);
    
    void Close();
private:
    IntraClient client;
};

class MasterIntraPool : public Pool<MasterIntra> {
protected:
    bool ProcessClient(MasterIntra client) override;
};
}

#endif
