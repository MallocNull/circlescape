#include <SDL.h>
#include <glm/vec2.hpp>
#include <glm/mat2x2.hpp>

#define GL3_PROTOTYPES 1
#include <GL/glew.h>

void setColor(float r, float g, float b, SDL_Window* window) {
    glClearColor(r, g, b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[]) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf(SDL_GetError());
        return -1;
    }
    atexit(SDL_Quit);

    auto window = SDL_CreateWindow(
        "SockScape Client",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_OPENGL
    );

    auto ctx = SDL_GL_CreateContext(window);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetSwapInterval(1);

#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    glewInit();
#endif

    bool running = true;
    while(running) {
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                running = false;

            if(event.type == SDL_WINDOWEVENT &&
               event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {

            }

            if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(window);

    return 0;
}