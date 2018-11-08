#ifndef SOSC_UTIL_INI_H
#define SOSC_UTIL_INI_H

#include <typeinfo>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "string.hpp"

namespace sosc {
namespace ini {
struct Field {
    enum kType {
        STRING,
        INT32,
        UINT32,
        DOUBLE,
        BOOL
    };

    explicit Field
        (const std::string& name, kType type = STRING)
        : name(name), type(type) {}
    bool Test() const;

    std::string name;
    kType type;
};

struct Rule {
    Rule() = delete;
    Rule(
        const std::string& name,
        bool required,
        bool allow_multiple,
        const std::vector<Field>& required_fields
    ) : name(name),
        required(required),
        allow_multiple(allow_multiple),
        required_fields(required_fields) {};

    std::string name;
    bool required;
    bool allow_multiple;
    std::vector<Field> required_fields;
};

class File {
public:
    struct Proxy {
        explicit Proxy(const std::string& value) : value(value) {};

        explicit operator bool() const {
            if(this->value == "1" || this->value == "true")
                return true;
            else if(this->value == "0" || this->value == "false")
                return false;
            else
                throw std::bad_cast();
        }

        template<
            typename T,
            typename Decayed = typename std::decay<T>::type,
            typename = typename std::enable_if<
                !std::is_same<const char*, Decayed>::value
             && !std::is_same<std::allocator<char>, Decayed>::value
             && !std::is_same<std::initializer_list<char>, Decayed>::value
            >::type
        > explicit operator T() const {
            std::stringstream ss;
            ss << this->value;

            T retval;
            if(!(ss >> retval))
                throw std::bad_cast();
            else
                return retval;
        }

        std::string value;
    };

    class SectionList {
    public:
        class Section {
        public:
            bool HasKey(std::string name) const;
            const Proxy& operator[] (std::string key) const;
        private:
            std::map<std::string, std::string> values;
            friend class File;
        };

        bool HasKey(std::string name) const;
        int SectionCount() const;

        const Proxy& operator[] (std::string key) const;
        const Section& operator[] (int index) const;
    private:
        std::vector<Section> sections;
        friend class File;
    };

    File(const File&) = delete;
    static File* Open
        (const std::string& filename, const std::vector<Rule>& rules = {});

    bool HasSection(std::string name) const;
    void Close();

    const SectionList& operator[] (std::string name) const;
private:
    File() = default;
    static std::runtime_error LoadError
        (File* file, int line, const std::string& error);

    std::string filename;
    std::map<std::string, SectionList> section_lists;
};
}}

#endif
