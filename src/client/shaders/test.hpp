#ifndef SOSC_SHADER_TEST_H
#define SOSC_SHADER_TEST_H

#include <SDL.h>
#include "common.hpp"
#include "_shader.hpp"

namespace sosc {
namespace shdr {
class TestShader : public Shader {
protected:
    void PrepareLoad() override {
        this->AttachSource(SOSC_RESC("shaders/test.vert"), GL_VERTEX_SHADER);
        this->AttachSource(SOSC_RESC("shaders/test.frag"), GL_FRAGMENT_SHADER);
        this->LinkProgram();
    }
};
}}

#endif
