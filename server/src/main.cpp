#include <iostream>
#include <string>
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
    
    //std::string a = sosc::cgc::sha1("test", true);
    
    /*sosc::cgc::Blowfish fish("TESTKEY");
    
    std::string test = fish.Encrypt("imagine a test");
    std::string testd = fish.Decrypt(test);
    
    uint32_t teest = sosc::csprng::next<uint32_t>();
    std::cout << std::hex << teest;*/
    
    /*std::string hash = sosc::cgc::bcrypt_hash("test pwd");
    std::cout << hash << std::endl;
    std::cout << sosc::cgc::bcrypt_check("test pwd", hash);*/
    
    sosc::BigUInt a, b;
    
    a.Parse("ff0000");
    b.Parse("fff");
    //assert(a - b == sosc::BigUInt("feff01"));
    
    //a += b;
    a *= b;
    
    std::cout << a.ToString() << std::endl;
    
    return 0;
}
