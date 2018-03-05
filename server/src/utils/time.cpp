#include "time.hpp"

std::tm sosc::clk::to_utc(sosc::time time) {
    time_t ctime = sosc::clock::to_time_t(time);
    return to_utc(&ctime);
}

std::tm sosc::clk::to_utc(const time_t* time) {
    static std::mutex mtx;
    mtx.lock();
    
    std::tm     utc = std::tm(),
        *utc_static = std::gmtime(time);
    std::memcpy(&utc, utc_static, sizeof(std::tm)); 
    
    mtx.unlock();
    return utc;
}

std::tm sosc::clk::to_local(sosc::time time) {
    time_t ctime = sosc::clock::to_time_t(time);
    return to_local(&ctime);
}

std::tm sosc::clk::to_local(const time_t* time) {
    static std::mutex mtx;
    mtx.lock();
    
    std::tm    ctm = std::tm(),
        *tm_static = std::localtime(time);
    std::memcpy(&ctm, tm_static, sizeof(std::tm));
    
    mtx.unlock();
    return ctm;
}

std::tm sosc::clk::to_tm(sosc::time time) {
    return to_local(time);
}

std::tm sosc::clk::to_tm(const time_t* time) {
    return to_local(time);
}

sosc::time sosc::clk::error_time() {
    std::tm error = std::tm();
    error.tm_year = -1900;
    error.tm_mday = 1;
    error.tm_mon = error.tm_hour
        = error.tm_min = error.tm_sec = 0;
    
    return sosc::clock::from_time_t(std::mktime(&error));
}

bool sosc::clk::is_error_time(std::string data, size_t offset) {
    return (((data[offset] & 0xFF) << 4) | 
            ((data[offset + 1] & 0xF0) >> 4)) == 0;
}

bool sosc::clk::is_error_time(sosc::time time) {
    std::tm err_check = to_tm(time);
    return err_check.tm_year == -1900;
}

sosc::time sosc::clk::unpack_time(std::string data, size_t offset) {
    if(offset + 6 >= data.length())
        return error_time();
    
    struct tm time = tm();
    time.tm_year = ((data[offset] & 0xFF) << 4 | 
                    (data[offset + 1] & 0xF0) >> 4) - 1900;
    time.tm_mon  = data[offset + 1] & 0x0F;
    time.tm_mday = (data[offset + 2] & 0xFF) + 1;
    time.tm_hour = data[offset + 3] & 0xFF;
    time.tm_min  = data[offset + 4] & 0xFF;
    time.tm_sec  = data[offset + 5] & 0xFF;
    
    return sosc::clock::from_time_t(std::mktime(&time));
}

std::string sosc::clk::pack_time() {
    return pack_time(sosc::clock::now());
}

std::string sosc::clk::pack_time(sosc::time time) {
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

std::string sosc::clk::pack_error_time() {
    return std::string(6, 0);
}
