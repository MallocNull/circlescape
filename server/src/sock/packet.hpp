#ifndef SOSC_PACKET_H
#define SOSC_PACKET_H

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
    int Parse(const std::string& data, std::string* extra = nullptr);
    bool Check(int region_count, ...);
    
    inline uint8_t GetId() const {
        return this->id;
    }
    
    inline std::string operator []
        (const std::vector<std::string>::size_type index) const
    {
        return regions[index];
    }
private:
    uint8_t id;
    std::vector<std::string> regions;
};
}

#endif
