#include "editor.hpp"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
    auto window = SDL_CreateWindow("House generator", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    auto context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(0);
    gladLoadGL(SDL_GL_GetProcAddress);

    Editor editor(window, context);
    editor.Run();
    return 0;
}
