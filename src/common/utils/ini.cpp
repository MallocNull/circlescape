#include "ini.hpp"
using namespace sosc::ini;

File* File::Open
    (const std::string& filename, const std::vector<Rule>& rules)
{
    auto ini = new File;
    ini->filename = filename;
    std::ifstream file;

    file.open(filename);
    if(!file.is_open())
        throw LoadError(ini, -1,
            "Could not open file."
        );

    int line_number = 0;
    std::string line, tmp;
    File::SectionList::Section* active_section = nullptr;
    while(std::getline(file, line)) {
        ++line_number;
        str::trim(&line);
        if(line == "")
            continue;
        if(line[0] == ';')
            continue;

        if(line[0] == '[') {
            str::tolower(&line);
            if(line[line.length() - 1] != ']' || line.length() == 2)
                throw LoadError(ini, line_number,
                    "Section header must begin with [ and end with ]."
                );

            tmp = line.substr(1, line.length() - 2);
            if(ini->section_lists.count(tmp) == 0)
                ini->section_lists[tmp] = SectionList();

            auto sections = &(ini->section_lists[tmp].sections);
            sections->push_back(SectionList::Section());
            active_section = &((*sections)[sections->size() - 1]);
        } else {
            if(active_section == nullptr)
                throw LoadError(ini, line_number,
                    "Key-value pair field must be after section header."
                );

            auto parts = str::split(line, '=', 2);
            if(parts.size() < 2)
                throw LoadError(ini, line_number,
                    "Key-value pair field must be separated by '='."
                );

            str::tolower(str::trim(&(parts[0])));
            str::trim(&(parts[1]));

            if(parts[0] == "" || active_section->values.count(parts[0]) == 0)
                throw LoadError(ini, line_number,
                    "Duplicate key-value pair field in section."
                );

            active_section->values[parts[0]] = parts[1];
        }
    }

    for(auto& rule : rules) {
        if(rule.required && ini->section_lists.count(rule.name) == 0)
            throw LoadError(ini, -1, str::join({
                "Required section '", rule.name, "' not found."
            }));

        if(!rule.allow_multiple && ini[rule.name].sections.size() > 1)
            throw LoadError(ini, -1, str::join({
                "Multiple instances of '", rule.name, "' section "
                "when explicitely not allowed."
            }));

        for(auto& section : ini[rule.name].sections) {
            for(auto &field : rule.required_fields) {
                str::tolower(&field.name);
                if(section.values.count(field.name) == 0)
                    throw LoadError(ini, -1, str::join({
                        "Required field '", field.name, "' in section '",
                        rule.name, "' not found."
                    }));

                if(!field.Test())
                    throw LoadError(ini, -1, str::join({
                        "Field '", field.name, "' in section '",rule.name, "' "
                        "cannot be casted to requested type."
                    }));
            }
        }
    }

    return ini;
}

std::runtime_error File::LoadError
    (File* file, int line, const std::string &error)
{
    delete file;
    if(line > 0)
        return std::runtime_error(str::join(
            {"LOAD ERROR IN '", file->filename, "' L", TOSTR(line), ": ", error}
        ));
    else
        return std::runtime_error(str::join(
            {"LOAD ERROR IN '", file->filename, "': ", error}
        ));
}

const File::SectionList&
    File::operator[] (std::string name) const
{
    str::tolower(&name);
    if(this->section_lists.count(name) > 0)
        return this->section_lists[name];
    else
        throw std::range_error("Section name not found.");
}

const std::string&
    File::SectionList::operator[] (std::string key) const
{
    return this->sections[0][key];
}

const File::SectionList::Section&
    File::SectionList::operator[] (int index) const
{
    if(index < sections.size())
        return this->sections[index];
    else
        throw std::range_error("Section index out-of-bounds.");
}

const std::string&
    File::SectionList::Section::operator[] (std::string key) const
{
    str::tolower(&key);
    if(this->values.count(key) > 0)
        return this->values[key];
    else
        throw std::range_error("Key not found in section.");
}

void File::Close() {
    delete this;
}