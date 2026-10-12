#include "editor.hpp"
#include <glad/gl.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <iostream>

#include "../external/imgui/backends/imgui_impl_sdl3.h"
#include "../external/imgui/backends/imgui_impl_opengl3.h"

#include "vertex.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "scene_object.hpp"
#include "event_system.hpp"
#include "line.hpp"

Editor::Editor() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
    _window = SDL_CreateWindow("House generator", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    _context = SDL_GL_CreateContext(_window);
    SDL_GL_SetSwapInterval(0);
    gladLoadGL(SDL_GL_GetProcAddress);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForOpenGL(_window, _context);
    ImGui_ImplOpenGL3_Init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
}

glm::vec3 Editor::PolarToWorld(glm::vec3 polar) {
    auto horizontal = glm::angleAxis(glm::radians(polar.x), glm::vec3(0, 1, 0));
    auto vertical = glm::angleAxis(glm::radians(polar.y), glm::vec3(1, 0, 0));
    return horizontal * vertical * glm::vec3(0, 0, polar.z) + glm::vec3(0, 0, 0);
}

glm::vec2 Editor::GetCursorNDC() {
    int screenWidth, screenHeight;
    SDL_GetWindowSize(_window, &screenWidth, &screenHeight);
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    return {
        mouseX / static_cast<float>(screenWidth) * 2 - 1,
        1 - mouseY / static_cast<float>(screenHeight) * 2
    };
}

void Editor::Run() {
    Vertex vertices[] = {
        Vertex{glm::vec3(-1, 0, -1), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
        Vertex{glm::vec3(1, 0, -1), glm::vec3(0, 0, 0), glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 2, -1), glm::vec3(0, 0, 0), glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 2, -1), glm::vec3(0, 0, 0), glm::vec2(1, 1)},

        Vertex{glm::vec3(-1, 0, 1), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
        Vertex{glm::vec3(1, 0, 1), glm::vec3(0, 0, 0), glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 2, 1), glm::vec3(0, 0, 0), glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 2, 1), glm::vec3(0, 0, 0), glm::vec2(1, 1)},

        Vertex{glm::vec3(1, 0, -1), glm::vec3(1, 1, 1), glm::vec2(0, 0)},
        Vertex{glm::vec3(1, 0, 1), glm::vec3(0, 1, 0), glm::vec2(1, 0)},
        Vertex{glm::vec3(1, 2, -1), glm::vec3(0, 0, 1), glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 2, 1), glm::vec3(1, 0, 0), glm::vec2(1, 1)},

        Vertex{glm::vec3(-1, 0, 1), glm::vec3(1, 1, 1), glm::vec2(0, 0)},
        Vertex{glm::vec3(-1, 0, -1), glm::vec3(1, 0, 0), glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 2, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1)},
        Vertex{glm::vec3(-1, 2, -1), glm::vec3(0, 0, 1), glm::vec2(1, 1)},
    };

    GLuint indices[] = {
        0, 1, 3,
        0, 2, 3,
        4, 5, 7,
        4, 6, 7,
        8, 9, 11,
        8, 10, 11,
        12, 13, 15,
        12, 14, 15,
    };

    auto vert = std::vector(vertices, vertices + 16);
    auto ind = std::vector(indices, indices + 24);
    auto mesh = Mesh(vert, ind);

    auto shader = Shader("assets/shaders/vert.vert", "assets/shaders/frag.frag");
    auto texture = Texture::LoadTexture("assets/penguin.jpg");
    shader.SetTexture2D(GL_TEXTURE0, texture);

    float size = 40.0f;
    Vertex planeVertices[] = {
        Vertex{glm::vec3(-1, 0, -1) * size, glm::vec3(0, 0, 0), glm::vec2(0, 0)},
        Vertex{glm::vec3(1, 0, -1) * size, glm::vec3(0, 0, 0), glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 0, 1) * size, glm::vec3(0, 0, 0), glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 0, 1) * size, glm::vec3(0, 0, 0), glm::vec2(1, 1)}
    };

    GLuint planeIndicies[] = {
        0, 1, 3,
        0, 2, 3
    };

    auto planeVert = std::vector(planeVertices, planeVertices + 4);
    auto planeInd = std::vector(planeIndicies, planeIndicies + 6);
    auto planeMesh = Mesh(planeVert, planeInd);

    auto gridShader = Shader("assets/shaders/vert.vert", "assets/shaders/grid.frag");
    auto plane = SceneObject(planeMesh, gridShader);
    auto object = SceneObject(mesh, shader);


    std::vector<glm::vec3> points{
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 1),
        glm::vec3(3, 0, 0),
        glm::vec3(0, 0, -1),
    };

    Shader color("assets/shaders/vert.vert", "assets/shaders/color.frag");
    Line line(points, color);

    // Camera camera;
    glm::vec3 cameraPos = glm::vec3(0, 1, 8);
    glm::vec3 polarPosition = glm::vec3(0, 0, 5);

    _eventSystem.RegisterEvent(SDL_EVENT_QUIT, [this](auto &&) { _running = false; return false; });
    _eventSystem.RegisterEvent(SDL_EVENT_KEY_DOWN, [this](const SDL_Event *event) {
        if (event->key.key == SDLK_ESCAPE)
            _running = false;
        return false;
    });

    // Outline dragging
    bool dragOutline = false;
    int dragPointIndex = 0;
    glm::vec3 cursorPosition = glm::vec3(0, 0, 0);
    _eventSystem.RegisterEvent(SDL_EVENT_MOUSE_BUTTON_DOWN, [this, &dragOutline, &dragPointIndex, &cursorPosition, &points](const SDL_Event *event) {
        if (PointInRange(cursorPosition, points, &dragPointIndex)) {
            dragOutline = true;
            return true;
        }
        return false;
    });

    _eventSystem.RegisterEvent(SDL_EVENT_MOUSE_BUTTON_UP, [&dragOutline](const SDL_Event *event) {
        if (event->button.button == SDL_BUTTON_LEFT)
            dragOutline = false;
        return false;
    });

    _eventSystem.RegisterEvent(SDL_EVENT_MOUSE_MOTION, [&dragOutline, &points, &line, &dragPointIndex, &cursorPosition](const SDL_Event *event) {
        if (!dragOutline)
            return false;

        points[dragPointIndex] = cursorPosition;
        line.SetPoints(points);
        return true;
    });

    // Camera panning
    _eventSystem.RegisterEvent(SDL_EVENT_MOUSE_BUTTON_DOWN, [this](const SDL_Event *event) {
        if (event->button.button == SDL_BUTTON_LEFT)
            _cameraPan = true;
        return false;
    });

    _eventSystem.RegisterEvent(SDL_EVENT_MOUSE_BUTTON_UP, [this](const SDL_Event *event) {
        if (event->button.button == SDL_BUTTON_LEFT)
            _cameraPan = false;
        return false;
    });

    _eventSystem.RegisterEvent(SDL_EVENT_MOUSE_WHEEL, [&polarPosition](const SDL_Event *event) {
        polarPosition -= glm::vec3(0, 0, event->wheel.y);
        polarPosition.z = glm::clamp(polarPosition.z, 1.f, 25.f);
        return false;
    });

    _eventSystem.RegisterEvent(SDL_EVENT_MOUSE_MOTION, [this, &polarPosition](const SDL_Event *event) {
        if (!_cameraPan)
            return false;
        polarPosition -= glm::vec3(event->motion.xrel / 10.0, event->motion.yrel / 10.0, 0);
        polarPosition.y = glm::clamp(polarPosition.y, -90.f + 1, 90.f - 1);
        return false;
    });

    _eventSystem.RegisterEvent(SDL_EVENT_WINDOW_RESIZED, [this](const SDL_Event *event) {
        int width, height;
        SDL_GetWindowSize(_window, &width, &height);
        glViewport(0, 0, width, height);
        _camera.SetAspectRatio((float) width / (float) height);
        return false;
    });

    auto lastFrame = SDL_GetTicksNS();
    _running = true;
    while (_running) {
        _eventSystem.ProcessEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Performance", &_running,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::SetWindowPos({10, 10});

        int width, height;
        SDL_GetWindowSize(_window, &width, &height);
        ImGui::SetWindowSize({width * 0.25f, height - 20.0f});
        auto deltaMilis = (SDL_GetTicksNS() - lastFrame) / 1000000.0f;
        lastFrame = SDL_GetTicksNS();
        ImGui::Text("Frametime: %lf ms", deltaMilis);

        cameraPos = PolarToWorld(polarPosition);
        ImGui::DragFloat3("camera", &cameraPos[0], 0.01f);
        ImGui::DragFloat3("model", &polarPosition[0], 0.01f);
        ImGui::Spacing();
        ImGui::Text("Arrays");

        int index = points.size();
        if (ImGui::InputInt("asd", &index)) {
            if (index < points.size()) {
                points.pop_back();
            } else {
                points.push_back(glm::vec3(0, 0, 0));
            }
        }


        for (int i = 0; i < points.size(); i++) {
            ImGui::DragFloat3(std::to_string(i).c_str(), &(points[i])[0], 0.01f);
        }

        ImGui::End();
        // ImGui::ShowDemoWindow();
        ImGui::Render();

        // Testing cursor to world coordinates here
        auto point = GetPointOnPlane();
        cursorPosition = point;

        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto pos = PolarToWorld(polarPosition);
        _camera.SetPosition(pos);
        _camera.LookAt(glm::vec3(0, 0, 0));

        line.SetPoints(points);
        line.Draw(_camera);

        plane.Draw(_camera);
        object.Draw(_camera);

        auto error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cout << "ERROR: " << error << std::endl;
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(_window);
    }

    SDL_GL_DestroyContext(_context);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

bool Editor::PointInRange(glm::vec3 cursor, std::vector<glm::vec3> points, int *targetPoint) {
    float threshold = 0.1;
    float closest = FLT_MAX;
    int closestPoint = -1;

    for (auto i = 0; i < points.size(); i++) {
        auto point = points[i];
        auto distance = glm::distance(cursor, point);
        if (distance > closest)
            continue;
        closest = distance;
        closestPoint = i;
    }

    if (closest < threshold) {
        if (targetPoint != nullptr) {
            *targetPoint = closestPoint;
        }
        return true;
    }
    return false;
}

glm::vec3 Editor::GetPointOnPlane() {
    auto mousePos = GetCursorNDC();
    auto cursorDirection = _camera.ScreenToDirection(mousePos);
    auto t = _camera.GetPosition().y / cursorDirection.y;
    return _camera.GetPosition() - cursorDirection * t;
}
