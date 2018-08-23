#ifndef SOSC_SHADER_CORE_H
#define SOSC_SHADER_CORE_H

#include "common.hpp"
#include <GL/glew.h>
#include <vector>
#include <exception>
#include <sstream>
#include <fstream>

namespace sosc {
struct ShaderCompilationException : public std::exception {
public:
    ShaderCompilationException
        (const std::string& shaderName, const std::string& errorInfo)
    {
        std::stringstream ss;
        ss << "Error in '" << shaderName << "': " << errorInfo;
        this->errorInfo = ss.str();
    }

    const char* what() noexcept {
        return this->errorInfo.c_str();
    }
private:
    std::string errorInfo;
};

struct ShaderUniformException : public std::exception {
public:
    explicit ShaderUniformException(const std::string& uniformName) {
        std::stringstream ss;
        ss << "Could not find uniform '" << uniformName << "'.";
        this->errorInfo = ss.str();
    }

    const char* what() noexcept {
        return this->errorInfo.c_str();
    }
private:
    std::string errorInfo;
};

class Shader {
public:
    Shader();

    bool Load();
    void Start() const;

    inline GLint operator[] (std::vector<GLint>::size_type index) {
        return locations[index];
    }

    inline GLint Uniform(std::vector<GLint>::size_type index) {
        return locations[index];
    }

    void Stop() const;
    void Unload();
protected:
    virtual bool PrepareLoad() = 0;
    virtual void PrepareUnload() {};

    void AttachSource(const std::string& fileName, GLuint shaderType);
    void LinkProgram();
    void LoadUniforms(std::vector<std::string> names);
private:
    std::vector<GLint> locations;
    std::vector<GLuint> shaders;
    GLuint program;
    bool loaded;
};
}

#endif
