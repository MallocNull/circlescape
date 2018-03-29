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

int main(int argc, char **argv) {
    sosc::TcpServer server;
    sosc::TcpClient client;
    std::string buffer;
    
    server.Listen(8080);
    std::cout << "Listening ..." << std::endl;
    server.Accept(&client);
    std::cout << "Reading ..." << std::endl;
    
    while(true) {
        client.Receive(&buffer, SOSC_TCP_BLOCK);
        std::cout << buffer;
    }
    
    return 0;
}
