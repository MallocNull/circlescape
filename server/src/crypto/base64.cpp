#include "base64.hpp"

#define B64_IGNORE  0xFE
#define B64_PADDING 0xFF

std::string sosc::cgc::base64_encode(const std::string& data) {
    return base64_encode(data.c_str(), data.length());
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

static uint8_t base64_decode_char(char c) {
    if(c >= 'A' && c <= 'Z')
        return c - 'A';
    else if(c >= 'a' && c <= 'z')
        return c - 'a' + 26;
    else if(c >= '0' && c <= '9')
        return c - '0' + 52;
    else if(c == '+')
        return 62;
    else if(c == '/')
        return 63;
    else if(c == '=')
        return B64_PADDING;
    else
        return B64_IGNORE;
}

static void base64_decode_append(std::string* decoded, int data) {
    *decoded += (char)(data >> 16);
    
    if((data & (1 << 30)) == 0)
        *decoded += (char)(data >> 8);
    
    if((data & (1 << 29)) == 0)
        *decoded += (char)data;
}

std::string sosc::cgc::base64_decode(const std::string& data) {    
    std::string decoded;
    int index = 0, j = 0;
    
    for(std::string::size_type i = 0; i < data.length(); ++i) {
        int sextet = base64_decode_char(data[i]);
            
        if(sextet == B64_IGNORE || sextet == B64_PADDING) {
            if(sextet == B64_PADDING && i >= data.length() - 2 && j > 1)
                index |= 1 << (31 - j + 1);
            
            continue;
        }
        
        index |= sextet << (6 * (3 - j));
    
        j = (j + 1) % 4;
        if(j == 0) {
            base64_decode_append(&decoded, index);
            index = 0;
        }
    }
    
    if(j > 1) {
        index |= (j == 2) ? 3 << 29 : 1 << 29;
        base64_decode_append(&decoded, index);
    }
    
    return decoded;
}
