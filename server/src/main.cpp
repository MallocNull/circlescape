#include <iostream>
#include <string>
#include <ctime>
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

class User; 
class Test : sosc::Pool<User> {
protected:
    bool ProcessClient(User* client) override;
};

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
