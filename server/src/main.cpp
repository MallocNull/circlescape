#include <iostream>
#include <string>
#include "sock/tcpsock.hpp"
#include "utils/string.hpp"
#include "utils/net.hpp"
#include "utils/time.hpp"
#include "sock/tcpsock.hpp"

int main(int argc, char **argv) {
    //auto sock = sosc::TcpClient();
    
    /*std::string a = "this!!is!!a!!test";
    auto b = sosc::str::split(a, "!!");
    std::for_each(b.begin(), b.end(), [](std::string& i) {
        std::cout << i << std::endl;
    });*/
    
    sosc::TcpClient client;
    sosc::TcpServer server;
    
    server.Listen(1111);
    server.Accept(&client);
    
    client.Send("test");
    std::string got;

    while(client.IsOpen()) {
        int length = client.Recv(&got);
        
        if(length > 0)
            std::cout << got << std::endl;
    }
    
    return 0;
}
