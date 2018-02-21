#include "string.hpp"

std::string sosc::str::trim(std::string str) {
    return rtrimr(ltrimr(str));
}
std::string* sosc::str::trim(std::string* str) {
    return rtrim(ltrim(str));
}

std::string& sosc::str::trimr(std::string& str) {
    return rtrimr(ltrimr(str));
}

std::string sosc::str::rtrim(std::string str) {
    int marker = 0;
    
}
