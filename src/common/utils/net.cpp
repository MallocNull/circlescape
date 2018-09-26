#include "net.hpp"

// BEGIN IPADDRESS CLASS

sosc::net::IpAddress::IpAddress() {
    this->Reset();
}

void sosc::net::IpAddress::Reset() {
    for(int i = 0; i < 8; ++i)
        this->parts[i] = std::make_pair<uint16_t, uint8_t>(0, 0);
}

bool sosc::net::IpAddress::ParseError() {
    this->Reset();
    return false;
}

bool sosc::net::IpAddress::Parse(const std::string& addr) {
    std::string addr_trim = str::trim(addr);
    
    auto parts = str::split(addr_trim, '.');
    if(parts.size() == 4)
        return ParseIPv4Parts(parts) 
            ? true : ParseError();
    else if(parts.size() != 1)
        return ParseError();
    
    parts = str::split(addr_trim, "::");
    if(parts.size() == 1) {
        if(ParseIPv6Part(parts[0], true) != 8)
            return ParseError();
    } else if(parts.size() == 2) {
        int left_cnt, right_cnt;
        if((left_cnt = ParseIPv6Part(parts[0], true)) == -1)
            return ParseError();
        if((right_cnt = ParseIPv6Part(parts[1], false)) == -1)
            return ParseError();
        if(left_cnt + right_cnt > 8)
            return ParseError();
    } else
        return ParseError();
    
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
        
        if(part.empty())
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
    if(str::trim(addr_part).empty())
        return 0;
    
    auto parts = str::split(addr_part, ':');
    if(parts.size() > 8)
        return -1;
    
    for(int i = 0; i < parts.size(); ++i) {
        addrpart_t* ptr = 
            &(this->parts[from_start ? i : 8 - parts.size() + i]);
        std::string part = str::trim(parts[i]);
        auto part_len = (int)part.length();
        if(part_len > 4 || part_len == 0)
            return -1;
        
        if(part == "*") {
            ptr->first = 0;
            ptr->second = 0x0F;
            continue;
        }
        
        for(int j = 0; j < part_len; ++j) {
            if(part[j] == '*') {
                part[j] = '0';
                ptr->second |= 1 << (part_len - j - 1);
            } else if(!(part[j] >= 'a' && part[j] <= 'f')
                   && !(part[j] >= 'A' && part[j] <= 'F')
                   && !(part[j] >= '0' && part[j] <= '9'))
            {
                return -1;
            }
        }
        
        ptr->first = (uint16_t)std::stoi(part, nullptr, 16);
    }
    
    return (int)parts.size();
}

sosc::net::IpAddress::operator std::string () const {
    return this->ToString();
}

std::string sosc::net::IpAddress::ToString(bool force_ipv6) const {
    // TODO THIS
    return "::";
}

bool sosc::net::IpAddress::operator == (const IpAddress& rhs) const {
    for(int i = 0; i < 8; ++i) {
        uint8_t wildcards = this->parts[i].second | rhs.parts[i].second;
        uint16_t mask = 0xFFFF;
        
        if(wildcards != 0) {
            for(int j = 0; j < 4; ++j) {
                if((wildcards & (1 << j)) != 0)
                    mask &= ~(0xF << (4*j));
            }
        }
        
        if((this->parts[i].first & mask) != (rhs.parts[i].first & mask))
            return false;
    }
    
    return true;
}

bool sosc::net::IpAddress::operator != (const IpAddress& rhs) const {
    return !(*this == rhs);
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

    return this->parts[5] == std::make_pair<uint16_t, uint8_t>(0xFFFF, 0);
}

// END IPADDRESS CLASS

bool sosc::net::is_big_endian() {
    static uint16_t check = 0xFF00;
    if(check == 0xFF00)
        check = *((uint8_t*)&check) == 0xFF;
    
    return check;
}
