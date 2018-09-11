#include <SDL.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <emscripten.h>

#include "ui/font.hpp"
#include "shaders/test.hpp"

void setColor(float r, float g, float b, SDL_Window* window) {
    glClearColor(r, g, b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
}

void setupOrthoMode() {

}

void draw();

sosc::ui::Text text;
SDL_Window* window;

int main(int argc, char* argv[]) {
    using namespace sosc;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf(SDL_GetError());
        return -1;
    }
    atexit(SDL_Quit);

    window = SDL_CreateWindow(
        "SockScape Client",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_OPENGL
    );

    SDL_GL_LoadLibrary(nullptr);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetSwapInterval(1);

    auto ctx = SDL_GL_CreateContext(window);
    if(ctx == nullptr)
        return -1;

#ifndef __APPLE__
    if(glewInit() != GLEW_OK)
        return -1;
#endif

    ui::font_init_subsystem(window);
    ui::Font scapeFont(
        SOSC_RESC("fonts/scape.bmp"),
        SOSC_RESC("fonts/scape.dat")
    );
    ui::font_set_default(&scapeFont);

    text = ui::Text(75, glm::vec4(1, 0, 0, 1), "test", 100, 100, 400);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    emscripten_set_main_loop(draw, 30, 1);

    /*ui::font_deinit_subsystem();
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(window);*/

    return 0;
}

void draw() {
    using namespace sosc;

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(.25, .25, .25, 1);

    text.Render();

    SDL_GL_SwapWindow(window);

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        /*if(event.type == SDL_QUIT)
            running = false;*/

        if(event.type == SDL_WINDOWEVENT &&
           event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        {
            glViewport(0, 0, event.window.data1, event.window.data2);
            ui::font_window_changed(window);
        }

        if(event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.sym) {
                /*case SDLK_ESCAPE:
                    running = false;
                    break;*/
                default:
                    break;
            }
        }
    }
}