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
        if((*str)[marker] > 0x20) break;
        
    str->erase(0, marker);
    return str;
}

std::string sosc::str::rtrim(std::string str) {
    return *rtrim(&str);
}

std::string* sosc::str::rtrim(std::string *str) {
    int marker = 0;
    for(; marker < str->length(); ++marker)
        if((*str)[str->length() - marker - 1] > 0x20) break;
        
    str->erase(str->length() - marker, marker);
    return str;
}

std::vector<std::string> sosc::str::split
    (const std::string& str, char delimiter, int count)
{
    std::string buffer;
    std::stringstream stream(str);
    auto parts = std::vector<std::string>();
    
    --count;
    while(count != 0) {
        if(!std::getline(stream, buffer, delimiter))
            break;
        
        parts.push_back(buffer);
        --count;
    }
    
    if(std::getline(stream, buffer))
        parts.push_back(buffer);
    return parts;
}

std::vector<std::string> sosc::str::split
    (const std::string& str, std::string delimiter, int count)
{
    auto parts = std::vector<std::string>();
    std::size_t chunk_end = 0, chunk_start = 0;
    
    --count;
    while((chunk_end = str.find(delimiter, chunk_end)) 
        != std::string::npos && count != 0)
    {
        parts.push_back(str.substr(chunk_start, chunk_end - chunk_start));
        chunk_start = (chunk_end += delimiter.length());
        --count;
    }
    
    parts.push_back(str.substr(chunk_start, std::string::npos));
    return parts;
}

std::string sosc::str::join(const std::vector<std::string>& parts,
    char delimiter, int count)
{
    return join(parts, std::string(1, delimiter), count);
}

std::string sosc::str::join(const std::vector<std::string>& parts,
    std::string delimiter, int count)
{
    std::string assembled;
    int bounds = (count == -1)
        ? parts.size()
        : std::min<int>(count, parts.size());
        
    for(int i = 0; i < bounds; ++i)
        assembled += (i == 0 ? "" : delimiter) + parts[i];
    
    return assembled;
}

bool sosc::str::starts
    (const std::string& str, const std::string& start) 
{
    if(start.length() > str.length())
        return false;
    
    return std::mismatch(
            start.begin(), start.end(), str.begin()
        ).first == start.end();
}

bool sosc::str::ends
    (const std::string& str, const std::string& end) 
{
    if(end.length() > str.length())
        return false;
    
    return std::mismatch(
            end.begin(), end.end(), str.end() - end.length()
        ).first == end.end();
}

bool sosc::str::contains
    (const std::string& haystack, const std::string& needle) 
{
    return haystack.find(needle) != std::string::npos;
}

std::string sosc::str::tolower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

std::string* sosc::str::tolower(std::string* str) {
    std::transform(str->begin(), str->end(), str->begin(), ::tolower);
    return str;
}
