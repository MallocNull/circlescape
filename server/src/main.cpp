#include <iostream>
#include <string>
#include <ctime>
#include <thread>
#include "sock/tcpsock.hpp"
#include "utils/string.hpp"
#include "utils/net.hpp"
#include "utils/time.hpp"
#include "sock/tcpsock.hpp"
#include "crypto/sha1.hpp"
#include "crypto/base64.hpp"
#include "crypto/bfish.hpp"
#include "utils/csprng.hpp"
#include "crypto/bcrypt.hpp"
#include "utils/bigint.hpp"
#include "sock/scapesock.hpp"
#include "sock/pool.hpp"

#include "hosts/master.hpp"
#include "hosts/slave.hpp"

bool master_intra(uint16_t port, const sosc::poolinfo_t& info);
bool master_client(uint16_t port, const sosc::poolinfo_t& info);
bool slave(uint16_t port, const sosc::poolinfo_t& info);

int main(int argc, char **argv) {
    if(argc < 2)
        return -1;

    if(argv[1][0] == 'm') {
        master_intra(1234, sosc::poolinfo_t());
    } else {
        slave(1234, sosc::poolinfo_t());
    }

    return 0;
}

bool master_intra(uint16_t port, const sosc::poolinfo_t& info) {
    using namespace sosc;
    
    IntraServer server;
    IntraClient client;
    if(!server.Listen(port))
        return false;
    
    MasterIntraPool pool;
    pool.Configure(info);
    pool.Start();
    
    while(server.Accept(&client))
        pool.AddClient(MasterIntra(client));

    pool.Stop();
    return true;
}

bool master_client(uint16_t port, const sosc::poolinfo_t& info) {
    using namespace sosc;

    return true;
}

bool slave(uint16_t port, const sosc::poolinfo_t& info) {
    using namespace sosc;



    ScapeServer server;
    ScapeConnection client;
    if(!server.Listen(port))
        return false;

    SlaveClientPool pool;
    pool.Configure(info);
    pool.Start();

    while(server.Accept(&client))
        pool.AddClient(SlaveClient(client));

    pool.Stop();
    return true;
}
