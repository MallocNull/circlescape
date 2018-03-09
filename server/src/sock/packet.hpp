#ifndef SOSC_PACKET_H
#define SOSC_PACKET_H

#include <numeric>
#include <vector>
#include <string>
#include <cstdarg>
#include "../utils/net.hpp"

#define PCK_ANY 0

#define PCK_OK    0
#define PCK_MORE  1
#define PCK_ERR  -1

#define PCK_MAX_LEN 0xFFFFFFFF

namespace sosc {
class Packet {
public:
    bool AddRegion(std::string data);
    bool AddRegions(std::vector<std::string> data);
    
    void SetRegion(uint8_t index);
    void SetRegions(uint8_t start, std::vector<std::string> data);
    
    void DeleteRegion(uint8_t index);
    void DeleteRegions(uint8_t start, uint8_t length);
    inline void Clear() const {
        this->regions.clear();
    }
    
    int Parse(const std::string& data, std::string* extra = nullptr);
    bool Check(int region_count, ...);
    
    inline void SetId(uint8_t id) const {
        this->id = id;
    }
    
    inline uint8_t GetId() const {
        return this->id;
    }
    
    inline std::string operator []
        (const std::vector<std::string>::size_type index) const
    {
        return regions[index];
    }
    
    std::string ToString() const;
    inline operator std::string () const {
        return this->ToString();
    }
    inline operator const char* () const {
        return this->ToString().c_str();
    }
private:
    uint8_t id;
    std::vector<std::string> regions;
};
}

#endif