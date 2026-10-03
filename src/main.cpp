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

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"


struct vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 UV;
};

GLuint loadShader(std::string path, GLuint shader_type) {
    std::ifstream vertexShaderFile(path);
    std::stringstream buffer;
    buffer << vertexShaderFile.rdbuf();
    std::string source = buffer.str();
    const char* sources[] = { source.c_str() };
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, sources, nullptr);
    glCompileShader(shader);

    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint loadProgram() {
    auto vertexShader = loadShader("../assets/vert.vert", GL_VERTEX_SHADER);
    auto fragmentShader = loadShader("../assets/frag.frag", GL_FRAGMENT_SHADER);
    auto program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}



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

    vertex vertices[] = {
        // Front
        vertex{glm::vec3(-1,0,-1),glm::vec3(0,0,0), glm::vec2(0,0) },
        vertex{glm::vec3(1,0,-1),glm::vec3(0,0,0), glm::vec2(1,0) },
        vertex{glm::vec3(-1,2,-1), glm::vec3(0,0,0), glm::vec2(0,1) },
        vertex{glm::vec3(1,2,-1), glm::vec3(0,0,0), glm::vec2(1,1) },

        //Back
        vertex{glm::vec3(-1,0,1),glm::vec3(0,0,0), glm::vec2(0,0) },
        vertex{glm::vec3(1,0,1),glm::vec3(0,0,0), glm::vec2(1,0) },
        vertex{glm::vec3(-1,2,1), glm::vec3(0,0,0), glm::vec2(0,1) },
        vertex{glm::vec3(1,2,1), glm::vec3(0,0,0), glm::vec2(1,1) },

        vertex{glm::vec3(1,0,-1), glm::vec3(1,1,1), glm::vec2(0,0) },
        vertex{glm::vec3(1,0,1), glm::vec3(0,1,0), glm::vec2(1,0) },
        vertex{glm::vec3(1,2,-1),glm::vec3(0,0,1), glm::vec2(0,1) },
        vertex{glm::vec3(1,2,1), glm::vec3(1,0,0), glm::vec2(1,1) },

        vertex{glm::vec3(-1,0,1), glm::vec3(1,1,1), glm::vec2(0,0) },
        vertex{glm::vec3(-1,0,-1), glm::vec3(1,0,0), glm::vec2(1,0) },
        vertex{glm::vec3(-1,2,1), glm::vec3(0,1,0), glm::vec2(0,1) },
        vertex{glm::vec3(-1,2,-1),glm::vec3(0,0,1), glm::vec2(1,1) },
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
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    GLuint VAO,VBO,EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);

    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    auto *image = stbi_load("../assets/penguin.jpg", &width, &height, &channels,4);
    uint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);


    auto program = loadProgram();

    float angle = 0.0f;
    glm::vec3 cameraPos = glm::vec3(0, 1, 8);
    glm::vec3 modelPos = glm::vec3(0, 0, 0);

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
        auto label = "Frametime: " + std::to_string(deltaMilis);
        ImGui::Text(label.c_str());

        ImGui::SliderFloat("angle", &angle, 0.0f, 360.0f);
        ImGui::DragFloat3("camera", &cameraPos[0], 0.01f);
        ImGui::DragFloat3("model", &modelPos[0], 0.01f);

        ImGui::Render();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto identity = glm::identity<glm::mat4>();

        modelPos.y = glm::sin(SDL_GetTicks() / 250.0f) / 2 + 0.5f;
        auto model = glm::translate(identity, modelPos) * glm::rotate(identity, SDL_GetTicks() / 500.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        GLuint modelID = glGetUniformLocation(program, "model");
        glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);

        auto camera = glm::translate(identity, -cameraPos);
        GLuint cameraID = glGetUniformLocation(program, "camera");
        glUniformMatrix4fv(cameraID, 1, GL_FALSE, &camera[0][0]);

        glm::mat4 projection_matrix = glm::perspective(glm::radians(60 / 2.0), 16.0 / 9.0, 0.1, 100.0);
        GLuint projectionID = glGetUniformLocation(program, "projection");
        glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection_matrix[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glUseProgram(program);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6 * 4, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
