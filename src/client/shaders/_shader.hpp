#ifndef SOSC_SHADER_CORE_H
#define SOSC_SHADER_CORE_H

#include "common.hpp"
#include <GL/glew.h>
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

namespace sosc {
namespace shdr {
class Shader {
public:
    Shader();

    bool Load();
    void Start() const;

    inline GLint operator[] (std::vector<GLint>::size_type index) {
        if(index >= locations.size())
            return -1;

        return locations[index];
    }

    inline GLint Uniform(std::vector<GLint>::size_type index) {
        if(index >= locations.size())
            return -1;

        return locations[index];
    }

    void Stop() const;
    void Unload();
protected:
    virtual void PrepareLoad() = 0;
    virtual void PrepareUnload() {};

    bool AttachSource(const std::string& fileName, GLuint shaderType);
    bool LinkProgram();
    bool LoadUniforms(std::vector<std::string> names);
private:
    std::vector<GLint> locations;
    std::vector<GLuint> shaders;
    GLuint program;
    bool loaded;
};
}}

#endif
