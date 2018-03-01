#include <iostream>
#include <string>
#include "sock/tcpsock.hpp"
#include "utils/string.hpp"
#include "utils/net.hpp"

int main(int argc, char **argv) {
    //auto sock = sosc::TcpClient();
    
    /*std::string a = "this!!is!!a!!test";
    auto b = sosc::str::split(a, "!!");
    std::for_each(b.begin(), b.end(), [](std::string& i) {
        std::cout << i << std::endl;
    });*/
    
    uint32_t test = 0xDEADBEEF;
    std::string net = HTONUL(test);
    test = NTOHUL(net);
    
    std::cout << std::hex << test << std::hex << net << std::endl;
    
    return 0;
}
