#include "packet.hpp"

sosc::Packet::Packet() {
    this->id = 0;
}

sosc::Packet::Packet(uint8_t id, std::vector<std::string> data) {
    this->SetId(id);
    this->regions = data;
}

bool sosc::Packet::AddRegion(std::string data) {
    if(this->regions.size() >= 256)
        return false;
    
    this->regions.push_back(data);
    return true;
}

bool sosc::Packet::AddRegions(std::vector<std::string> data) {
    if(this->regions.size() + data.size() > 256)
        return false;

    for(const auto& i : data)
        this->regions.push_back(i);

    return true;
}

void sosc::Packet::SetRegion(uint8_t index, std::string data) {
    if(index >= this->regions.size())
        this->regions.resize(index + 1);
    
    this->regions[index] = data;
}

void sosc::Packet::SetRegions
    (uint8_t start, std::vector<std::string> data)
{
    if(start + data.size() > 256)
        return;
    
    if(start + data.size() > this->regions.size())
        this->regions.resize(start + data.size());
    
    for(int i = 0; i < data.size(); ++i)
        this->regions[start + i] = data[i];
}

void sosc::Packet::DeleteRegion(uint8_t index) {
    if(index >= this->regions.size())
        return;
    
    this->regions.erase(this->regions.begin() + index);
}

void sosc::Packet::DeleteRegions(uint8_t start, uint8_t length) {
    if(start + length > 256)
        return;
    
    for(int i = 0; i < length; ++i)
        this->regions.erase(this->regions.begin() + start);
}

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
    
    if(length > expected_length && extra != nullptr)
        *extra = data.substr(expected_length);
    else if(extra != nullptr)
        *extra = "";
    
    return PCK_OK;
}

bool sosc::Packet::Check(int region_count, ...) const {
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

std::string* sosc::Packet::ToString(std::string* packet) const {
    *packet = std::string(8, 0);
    (*packet)[0] = 0xB0;
    (*packet)[1] = 0x0B;
    (*packet)[6] = this->id;
    (*packet)[7] = regions.size();
    
    //for(auto i = this->regions.begin(); i != this->regions.end(); ++i) {
    for(const auto& i : this->regions) {
        if(i.size() < 0xFE)
            *packet += (char)i.size();
        else if(i.size() <= 0xFFFF) {
            *packet += (char)0xFE;
            *packet += net::htonv<uint16_t>(i.size());
        } else {
            *packet += (char)0xFF;
            *packet += net::htonv<uint32_t>(i.size());
        }
    }
    
    //for(auto i = this->regions.begin(); i != this->regions.end(); ++i)
    for(const auto& i : this->regions)
        *packet += i;
    
    packet->assign(net::htonv<uint32_t>(packet->length()), 2, 4);
    
    return packet;
}

std::string sosc::Packet::ToString() const {
    std::string packet;
    this->ToString(&packet);
    return packet;
}
