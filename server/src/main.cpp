#include <iostream>
#include <string>
#include "sock/tcpsock.hpp"
#include "utils/string.hpp"

int main(int argc, char **argv) {
    //auto sock = sosc::TcpClient();
    
    std::string a = "this!!is!!a!!test";
    auto b = sosc::str::split(a, "!!");
    std::for_each(b.begin(), b.end(), [](std::string& i) {
        std::cout << i << std::endl;
    });
    
    return 0;
}
