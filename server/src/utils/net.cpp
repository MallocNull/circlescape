#include "net.hpp"

// BEGIN IPADDRESS CLASS

sosc::net::IpAddress::IpAddress() {
    for(int i = 0; i < 8; ++i)
        this->parts[i] = std::make_pair(0, 0);
}

bool sosc::net::IpAddress::Parse(const std::string& addr) {
    std::string addr_trim = str::trim(addr);
    
    auto parts = str::split(addr_trim, '.');
    if(parts.size() == 4)
        return ParseIPv4Parts(parts);
    else if(parts.size() != 1)
        return false;
    
    parts = str::split(addr_trim, "::");
    if(parts.size() == 1) {
        if(ParseIPv6Part(parts[0], true) != 8)
            return false;
    } else if(parts.size() == 2) {
        int left_cnt, right_cnt;
        if((left_cnt = ParseIPv6Part(parts[0], true)) == -1)
            return false;
        if((right_cnt = ParseIPv6Part(parts[1], false)) == -1)
            return false;
        if(left_cnt + right_cnt > 8)
            return false;
    } else
        return false;
    
    return true;
}

bool sosc::net::IpAddress::ParseIPv4Parts
    (const std::vector<std::string>& parts)
{
    this->parts[5] = std::make_pair(0xFFFF, 0);
    for(int i = 0; i < 4; ++i) {
        addrpart_t* ptr = &(this->parts[i < 2 ? 6 : 7]);
        std::string part = str::trim(parts[i]);
        int part_int;
        
        if(part == "")
            return false;
        else if(part == "*") {
            part_int = 0;
            ptr->second |= (i % 2 == 0) ? 0xC : 0x3;
        } else {
            try        { part_int = std::stoi(part); }
            catch(...) { return false; }
        }
        
        if(part_int < 0 || part_int > 255)
            return false;
        
        ptr->first |= (i % 2 == 0) 
            ? part_int << 8 
            : part_int;
    }
    
    return true;
}

int sosc::net::IpAddress::ParseIPv6Part
    (const std::string& addr_part, bool from_start) 
{
    if(str::trim(addr_part) == "")
        return 0;
    
    auto parts = str::split(addr_part, ':');
    if(parts.size() > 8)
        return -1;
    
    for(int i = 0; i < parts.size(); ++i) {
        addrpart_t* ptr = 
            &(this->parts[from_start ? i : 8 - parts.size() + i]);
        std::string part = str::trim(parts[i]);
        int part_len = part.length();
        if(part_len > 4 || part_len == 0)
            return -1;
        
        if(part == "*") {
            ptr->first = 0;
            ptr->second = 0x0F;
            continue;
        }
        
        for(int i = 0; i < part_len; ++i) {
            if(part[i] == '*') {
                part[i] = '0';
                ptr->second |= 1 << (part_len - i - 1);
            }
        }
        
        try        { ptr->first = std::stoi(part, NULL, 16); }
        catch(...) { return -1; }
    }
    
    return parts.size();
}

sosc::net::IpAddress::operator std::string () const {
    return this->ToString();
}

sosc::net::IpAddress::operator const char* () const {
    return this->ToString().c_str();
}

std::string sosc::net::IpAddress::ToString(bool force_ipv6) const {
    return "TODO THIS";
}

bool sosc::net::IpAddress::IsIdentical(const IpAddress& rhs) const {
    for(int i = 0; i < 8; ++i)
        if(this->parts[i] != rhs.parts[i])
            return false;
    
    return true;
}

bool sosc::net::IpAddress::IsIPv4() const {
    auto blank = std::make_pair<uint16_t, uint8_t>(0, 0);
    for(int i = 0; i < 5; ++i)
        if(this->parts[i] != blank)
            return false;
        
    if(this->parts[5] != std::make_pair<uint16_t, uint8_t>(0xFFFF, 0))
        return false;
    
    return true;
}

// END IPADDRESS CLASS

bool sosc::net::is_big_endian() {
    static uint16_t check = 0xFF00;
    if(check == 0xFF00)
        check = *((uint8_t*)&check) == 0xFF;
    
    return check;
}
