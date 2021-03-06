#ifndef SOSC_UTIL_STRING_H
#define SOSC_UTIL_STRING_H

#include <cstring>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#undef tostr
#undef TOSTR
#define TOSTR(X) std::to_string(X)

#undef bytestr
#undef BYTESTR
#define BYTESTR(X) std::string(1, X)

namespace sosc {
namespace str {    
std::string  trim (std::string  str);
std::string* trim (std::string* str);

std::string  ltrim (std::string  str);
std::string* ltrim (std::string* str);
    
std::string  rtrim (std::string  str);
std::string* rtrim (std::string* str);

std::vector<std::string> split
    (const std::string& str, char delimiter, int count = -1);
std::vector<std::string> split
    (const std::string& str, std::string delimiter, int count = -1);

std::string join(const std::vector<std::string>& parts, int count = -1);
std::string join(const std::vector<std::string>& parts, 
    char delimiter, int count = -1);
std::string join(const std::vector<std::string>& parts, 
    std::string delimiter, int count = -1);

bool starts(const std::string& str, const std::string& start);
bool ends(const std::string& str, const std::string& end);

bool contains(const std::string& haystack, const std::string& needle);

std::string  tolower(std::string  str);
std::string* tolower(std::string* str);

bool verify_email(const std::string& email);
}}

#endif
