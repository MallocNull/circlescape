#include "string.hpp"

std::string sosc::str::trim(std::string str) {
    return *rtrim(ltrim(&str));
}
std::string* sosc::str::trim(std::string* str) {
    return rtrim(ltrim(str));
}

std::string sosc::str::ltrim(std::string str) {
    return *ltrim(&str);
}

std::string* sosc::str::ltrim(std::string* str) {
    int marker = 0;
    for(; marker < str->length(); ++marker)
        if((*str)[marker] < 0x21) break;
        
    str->erase(0, marker - 1);
    return str;
}

std::string sosc::str::rtrim(std::string str) {
    return *rtrim(&str);
}

std::string* sosc::str::rtrim(std::string *str) {
    int marker = 0;
    for(; marker < str->length(); --marker)
        if((*str)[str->length() - marker - 1] < 0x21) break;
        
    str->erase(str->length() - marker - 1, marker);
    return str;
}
