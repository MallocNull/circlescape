#include "packet.hpp"

int sosc::Packet::Parse(const std::string& data, std::string* extra) {
    const unsigned char* raw = 
        (const unsigned char*)data.c_str();
    auto length = data.length();
    
    if(length < 2)
        return PCK_MORE;
    
    if(raw[0] != 0xB0 || raw[1] != 0x0B)
        return PCK_ERR;
    
    if(length < 6)
        return PCK_MORE;
    
    uint32_t expected_length = net::ntohv<uint32_t>(data, 2);
    if(expected_length > PCK_MAX_LEN || expected_length < 8)
        return PCK_ERR;
    if(length < expected_length)
        return PCK_MORE;
    
    this->id = data[6];
    std::vector<uint32_t> region_lengths;
    uint32_t region_count = data[7], 
        ptr = 8, body_length = 0;
    for(int i = 0; i < region_count; ++i) {
        if(ptr >= length)
            return PCK_ERR;
        
        uint32_t region_length;
        switch(raw[ptr]) {
            default:
                region_length = raw[ptr];
                break;
            case 254:
                if(ptr + 2 >= length)
                    return PCK_ERR;
                
                region_length = net::ntohv<uint16_t>(data, ptr + 1);
                break;
            case 255:
                if(ptr + 4 >= length)
                    return PCK_ERR;
                
                region_length = net::ntohv<uint32_t>(data, ptr + 1);
                break;
        }
        
        region_lengths.push_back(region_length);
        body_length += region_length;
    }
    
    if(body_length - ptr != 0)
        return PCK_ERR;
    
    for(int i = 0; i < region_count; ++i) {
        this->regions[i] = data.substr(ptr, region_lengths[i]);
        ptr += region_lengths[i];
    }
    
    if(length > expected_length)
        *extra = data.substr(expected_length);
    
    return PCK_OK;
}

bool sosc::Packet::Check(int region_count, ...) {
    if(region_count > 0xFF)
        return false;
    
    va_list args;
    va_start(args, region_count);
    for(int i = 0; i < region_count; ++i) {
        uint32_t length = va_arg(args, uint32_t);
        if(length != PCK_ANY && this->regions[i].length() != length)
            return false;
    }
    va_end(args);
    
    return true;
}
