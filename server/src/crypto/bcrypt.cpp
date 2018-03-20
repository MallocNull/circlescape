#include "bcrypt.hpp"

std::string sosc::cgc::bcrypt_hash(const std::string& input) {
    std::string salt = csprng::next_bytes(16),
        input_t = input.substr(0, 72);
    
    Blowfish bfish;
        
    std::stringstream stream;
    stream << "$2b$" << BCRYPT_COST << "$"
           << base64_encode(salt, true)
           << "b";
           
    return stream.str();
}
