#ifndef SOSC_UTIL_INI_H
#define SOSC_UTIL_INI_H

#include <vector>
#include <map>

namespace sosc {
namespace ini {
class File {
public:
    struct Rule {
        Rule() = delete;
        Rule(
            const std::string& name,
            bool required,
            bool allow_multiple,
            const std::vector<std::string>& required_fields
        ) : name(name),
            required(required),
            allow_multiple(allow_multiple),
            required_fields(required_fields) {};

        std::string name;
        bool required;
        bool allow_multiple;
        std::vector<std::string> required_fields;
    };

    class Section {

    };
private:
    std::map<std::string, Section>
};
}}

#endif
