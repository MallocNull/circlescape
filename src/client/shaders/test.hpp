#ifndef SOSC_SHADER_TEST_H
#define SOSC_SHADER_TEST_H

#include "_shader.hpp"

namespace sosc {
namespace shdr {
class TestShader : public Shader {
public:
    enum Uniforms {
        SCREEN_SIZE = 0,
        GRAPHIC_SAMPLER
    };
};
}}

#endif
