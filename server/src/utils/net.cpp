#include "net.hpp"

bool sosc::net::is_big_endian() {
    static uint16_t check = 0xFF00;
    if(check == 0xFF00)
        check = *((uint8_t*)&check) == 0xFF;
    
    return check;
}

template<typename T>
std::string sosc::net::htonv(T host_var) {
    int byte_count = sizeof(T);
    std::string net_var(byte_count, 0);
    
    for(int b = 0; b < byte_count; ++b) {
        int i = is_big_endian()
            ? b : byte_count - b - 1;
            
        net_var[i] = ((uint8_t*)&host_var)[b];
    }
    
    return net_var;
}

template<typename T>
T sosc::net::ntohv(std::string net_var, size_t offset) {
    int byte_count = sizeof(T);
    T host_var = 0;
    
    for(int b = 0; b < byte_count; ++b) {
        if(offset + b >= net_var.length())
            break;
        
        int i = is_big_endian()
            ? b : byte_count - b - i;
            
        ((uint8_t*)&host_var)[i] = net_var[offset + b];
    }
    
    return host_var;
}

std::tm to_utc(const time_t* time) {
    static std::mutex mtx;
    mtx.lock();
    
    std::tm     utc = tm(),
        *utc_static = std::gmtime(time);
    std::memcpy(&utc, utc_static, sizeof(std::tm)); 
    
    mtx.unlock();
    return utc;
}

bool sosc::net::is_error_time(std::string data, size_t offset) {
    return is_error_time(unpack_time(data, offset));
}

bool sosc::net::is_error_time(sosc::time time) {
    std::time_t ctime = sosc::clock::to_time_t(time);
}

std::string sosc::net::pack_time() {
    return pack_time(sosc::clock::now());
}

std::string sosc::net::pack_time(sosc::time time) {
    std::time_t ctime = sosc::clock::to_time_t(time);
    std::tm utc = to_utc(&ctime);
    int year = utc.tm_year + 1900;

    std::stringstream stream;
    stream << (char)((year & 0xFF0) >> 4)
           << (char)(((year & 0x00F) << 4) | utc.tm_mon)
           << (char)(utc.tm_mday - 1)
           << (char)utc.tm_hour << (char)utc.tm_min << (char)utc.tm_sec;
    return stream.str();
}

std::string sosc::net::pack_error_time() {
    return std::string(6, 0);
}

sosc::time unpack_time(std::string data, size_t offset) {
    if(offset + 6 >= data.length())
        return pack_error_time();
    
    struct tm time = tm();
    time.tm_year = ((data[offset] & 
}

