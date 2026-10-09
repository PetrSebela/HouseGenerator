#include <fstream>
#include <iostream>

#include <imgui.h>
#include <sstream>

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

int main() {
    SDL_Init(SDL_INIT_VIDEO);
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
    camera.SetPosition(cameraPos);

    bool running = true;
    auto lastFrame = SDL_GetTicksNS();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

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

        auto identity = glm::identity<glm::mat4>();

        modelPos.y = glm::sin(SDL_GetTicks() / 250.0f) / 2 + 0.5f;

        // auto model = glm::translate(identity, modelPos) * glm::rotate(identity, SDL_GetTicks() / 500.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        // shader.SetMatrix4x4("model", model);

        // auto camera = glm::inverse(glm::translate(identity, cameraPos));
        // shader.SetMatrix4x4("camera", camera);

        // glm::mat4 projection_matrix = glm::perspective(glm::radians(60 / 2.0), 16.0 / 9.0, 0.1, 100.0);
        // shader.SetMatrix4x4("view", projection_matrix * camera);

        // mesh.Draw();
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
