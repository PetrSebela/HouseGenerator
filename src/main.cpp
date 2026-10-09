#include <fstream>
#include <iostream>

#include <imgui.h>

#include "../external/imgui/backends/imgui_impl_sdl3.h"
#include "../external/imgui/backends/imgui_impl_opengl3.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "glm/ext/matrix_transform.hpp"

#include <SDL3/SDL.h>

#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "scene_object.hpp"
#include "event_system.hpp"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
    SDL_Window *window = SDL_CreateWindow("House generator", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE  );
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(0);
    gladLoadGL(SDL_GL_GetProcAddress);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();

    Vertex vertices[] = {
        Vertex{glm::vec3(-1,0,-1),glm::vec3(0,0,0), glm::vec2(0,0) },
        Vertex{glm::vec3(1,0,-1),glm::vec3(0,0,0), glm::vec2(1,0) },
        Vertex{glm::vec3(-1,2,-1), glm::vec3(0,0,0), glm::vec2(0,1) },
        Vertex{glm::vec3(1,2,-1), glm::vec3(0,0,0), glm::vec2(1,1) },

        Vertex{glm::vec3(-1,0,1),glm::vec3(0,0,0), glm::vec2(0,0) },
        Vertex{glm::vec3(1,0,1),glm::vec3(0,0,0), glm::vec2(1,0) },
        Vertex{glm::vec3(-1,2,1), glm::vec3(0,0,0), glm::vec2(0,1) },
        Vertex{glm::vec3(1,2,1), glm::vec3(0,0,0), glm::vec2(1,1) },

        Vertex{glm::vec3(1,0,-1), glm::vec3(1,1,1), glm::vec2(0,0) },
        Vertex{glm::vec3(1,0,1), glm::vec3(0,1,0), glm::vec2(1,0) },
        Vertex{glm::vec3(1,2,-1),glm::vec3(0,0,1), glm::vec2(0,1) },
        Vertex{glm::vec3(1,2,1), glm::vec3(1,0,0), glm::vec2(1,1) },

        Vertex{glm::vec3(-1,0,1), glm::vec3(1,1,1), glm::vec2(0,0) },
        Vertex{glm::vec3(-1,0,-1), glm::vec3(1,0,0), glm::vec2(1,0) },
        Vertex{glm::vec3(-1,2,1), glm::vec3(0,1,0), glm::vec2(0,1) },
        Vertex{glm::vec3(-1,2,-1),glm::vec3(0,0,1), glm::vec2(1,1) },
    };

    GLuint indices[] = {
        0,1,3,
        0,2,3,
        4,5,7,
        4,6,7,
        8,9,11,
        8,10,11,
        12,13,15,
        12,14,15,
    };

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    auto vert = std::vector(vertices, vertices + 16);
    auto ind = std::vector(indices, indices + 24);
    auto mesh = Mesh(vert,ind);

    auto shader = Shader("../assets/shaders/vert.vert", "../assets/shaders/frag.frag");
    auto texture = Texture::LoadTexture("../assets/penguin.jpg");
    shader.SetTexture2D(GL_TEXTURE0, texture);

    auto object = SceneObject(mesh, shader);
    Camera camera;

    float angle = 0.0f;
    glm::vec3 cameraPos = glm::vec3(0, 1, 8);
    glm::vec3 modelPos = glm::vec3(0, 0, 0);

    EventSystem eventSystem;

    auto lastFrame = SDL_GetTicksNS();
    bool running = true;
    eventSystem.RegisterEvent(SDL_EVENT_QUIT, [&running](auto&&){running = false;});
    eventSystem.RegisterEvent(SDL_EVENT_MOUSE_BUTTON_DOWN, [](const SDL_Event *event){std::cout << "test " << event->type << std::endl;});

    while (running) {
        eventSystem.ProcessEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        auto deltaMilis = (SDL_GetTicksNS() - lastFrame) / 1000000.0f;
        lastFrame = SDL_GetTicksNS();
        ImGui::Text("Frametime: %lf ms", deltaMilis);

        ImGui::SliderFloat("angle", &angle, 0.0f, 360.0f);
        ImGui::DragFloat3("camera", &cameraPos[0], 0.01f);
        ImGui::DragFloat3("model", &modelPos[0], 0.01f);

        ImGui::Render();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        modelPos.y = glm::sin(SDL_GetTicks() / 250.0f) / 2 + 0.5f;
        object.SetPosition(modelPos);
        // object.SetRotation(glm::vec3(0,SDL_GetTicks() / 15.0f,0));

        camera.SetPosition(cameraPos);
        camera.LookAt(glm::vec3(0,0,0));
        object.Draw(camera);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
