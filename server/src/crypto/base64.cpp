#include "base64.hpp"

std::string sosc::cgc::base64_encode(const std::string& data) {
    return base64_decode(data.c_str());
}

std::string sosc::cgc::base64_encode(const void* raw, size_t length) {
    uint8_t* data = (uint8_t*)raw;
    const std::string char_table
        = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        
    std::string encoded;
    for(size_t i = 0; i < length; i += 3) {
        if(i > 0 && ((i / 3) * 4) % 76 == 0)
            encoded += "\r\n";
        
        int index = 0, padding = 0;
        for(int j = 0; j < 3; ++j) {
            if(i + j < length)
                index += data[i + j] << (8 * (2 - j));
            else
                ++padding;
        }
                
        for(int j = 0; j < 4 - padding; ++j)
            encoded += char_table[(index >> (6 * (3 - j))) & 0x3F];
            
        for(int j = 0; j < padding; ++j)
            encoded += '=';
    }
    
    return encoded;
}

std::string base64_decode(const std::string& data) {
    
}
