#ifndef SOSC_SHADER_TEST_H
#define SOSC_SHADER_TEST_H

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "common.hpp"
#include "_shader.hpp"

namespace sosc {
namespace shdr {
class TestShader : public Shader {
public:
    enum Uniforms {
        ORTHO_MATRIX = 0,
        GRAPHIC_SAMPLER
    };

    void UpdateWindow(SDL_Window* window) {
        this->Start();

        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        glm::mat4 orthoMatrix = glm::ortho(0, width, height, 0);
        glUniformMatrix4fv(
            (*this)[ORTHO_MATRIX], 1, GL_FALSE, glm::value_ptr(orthoMatrix)
        );

        this->Stop();
    }
protected:
    void PrepareLoad() override {
        this->AttachSource(SOSC_RESC("shaders/test.vert"), GL_VERTEX_SHADER);
        this->AttachSource(SOSC_RESC("shaders/test.frag"), GL_VERTEX_SHADER);

        this->LinkProgram();
        this->LoadUniforms({
            "orthoMatrix",
            "graphicSampler"
        });
    }
};
}}

#endif
