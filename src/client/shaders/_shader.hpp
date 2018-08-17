#ifndef SOSC_SHADER_CORE_H
#define SOSC_SHADER_CORE_H

#include "common.hpp"
#include <GL/glew.h>
#include <cstdarg>

class Shader {
public:
    Shader();
    bool Load();

    void Start();
    void Stop();

    void Unload();
protected:
    virtual bool PrepareLoad() = 0;
    virtual void PrepareUnload() {};

    bool AttachSource();
    bool FetchUniforms(int count, GLint* ids, ...);
private:
    GLuint program;
};

#endif
