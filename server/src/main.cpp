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

bool master_intra(uint16_t port);
bool master_client(uint16_t port);
bool slave(uint16_t port);

int main(int argc, char **argv) {
    sosc::ScapeServer server;
    sosc::ScapeConnection client;
    std::string buffer;
    
    if(!server.Listen(8080)) {
        std::cout << "Listening failed." << std::endl;
        return -1;
    }
    std::cout << "Listening ..." << std::endl;
    
    bool check = server.Accept(&client);
    std::cout << "Shaking ..." << std::endl;
    
    bool loop = true;
    while(loop) {
        if(!client.Handshaked()) 
            client.Handshake();
        else {
            sosc::Packet pck;
            client.Receive(&pck, true);
            std::cout << pck.RegionCount() << std::endl;
        }
    }
    
    server.Close();
    return 0;
}

bool master_intra(uint16_t port, sosc::poolinfo_t info) {
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
    
    return true;
}

bool master_client(uint16_t port, sosc::poolinfo_t info) {
    /*
    auto pooler = std::thread([&]() {
        
    });
    */
}

bool slave(uint16_t port, sosc::poolinfo_t info) {
    
}
