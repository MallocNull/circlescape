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
    
    /*sosc::TcpClient client;
    client.Open("127.0.0.1", 1111);
    
    client.Send("test");
    std::string got = "abc";

    while(client.IsOpen()) {
        int length = client.Receive(&got);
        
        if(length > 0)
            std::cout << got << std::endl;
    }*/
    
    return 0;
}
