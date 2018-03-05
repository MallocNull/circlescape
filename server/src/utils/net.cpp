#include "net.hpp"

bool sosc::net::is_big_endian() {
    static uint16_t check = 0xFF00;
    if(check == 0xFF00)
        check = *((uint8_t*)&check) == 0xFF;
    
    return check;
}
