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

void Editor::RegisterCallbacks() {
    _eventSystem->RegisterEvent(SDL_EVENT_QUIT, [this](auto &&) { _running = false; return false; });

    _eventSystem->RegisterEvent(SDL_EVENT_KEY_DOWN, [this](const SDL_Event *event) {
        if (event->key.key == SDLK_ESCAPE)
            _running = false;
        return false;
    });

    // Camera panning
    _eventSystem->RegisterEvent(SDL_EVENT_MOUSE_BUTTON_DOWN, [this](const SDL_Event *event) {
        if (event->button.button == SDL_BUTTON_LEFT)
            _cameraPan = true;
        return false;
    });

    _eventSystem->RegisterEvent(SDL_EVENT_MOUSE_BUTTON_UP, [this](const SDL_Event *event) {
        if (event->button.button == SDL_BUTTON_LEFT)
            _cameraPan = false;
        return false;
    });

    _eventSystem->RegisterEvent(SDL_EVENT_MOUSE_WHEEL, [this](const SDL_Event *event) {
        _cameraOrbitPosition -= glm::vec3(0, 0, event->wheel.y);
        _cameraOrbitPosition.z = glm::clamp(_cameraOrbitPosition.z, 1.f, 25.f);
        return false;
    });

    _eventSystem->RegisterEvent(SDL_EVENT_MOUSE_MOTION, [this](const SDL_Event *event) {
        if (!_cameraPan)
            return false;
        _cameraOrbitPosition -= glm::vec3(event->motion.xrel / 10.0, event->motion.yrel / 10.0, 0);
        _cameraOrbitPosition.y = glm::clamp(_cameraOrbitPosition.y, -90.f + 1, 90.f - 1);
        return false;
    });

    _eventSystem->RegisterEvent(SDL_EVENT_WINDOW_RESIZED, [this](const SDL_Event *event) {
        int width, height;
        SDL_GetWindowSize(_window, &width, &height);
        glViewport(0, 0, width, height);
        _camera->SetAspectRatio((float) width / (float) height);
        return false;
    });
}

void Editor::DrawUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImGui::Begin("Performance", &_running, flags);

    ImGui::SetWindowPos({10, 10});
    int width, height;
    SDL_GetWindowSize(_window, &width, &height);
    ImGui::SetWindowSize({width * 0.25f, height - 20.0f});

    auto deltaMilis = (SDL_GetTicksNS() - _lastFrameTime) / 1000000.0f;
    _lastFrameTime = SDL_GetTicksNS();
    ImGui::Text("Frametime: %lf ms", deltaMilis);
    ImGui::Checkbox("Relax selected", &_footprint->Relax);
    ImGui::Checkbox("Snap to grid", &_footprint->SnapToGrid);
    ImGui::SliderFloat("Grid size", &_footprint->GridSize, 0.01, 1.0f);

    ImGui::End();

    // ImGui::ShowDemoWindow();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

glm::vec3 Editor::GetCursorWorldPosition() {
    auto mousePos = GetCursorNDC();
    auto cursorDirection = _camera->ScreenToDirection(mousePos);
    auto t = _camera->GetPosition().y / cursorDirection.y;
    return _camera->GetPosition() - cursorDirection * t;
}


Editor::Editor(SDL_Window *window, SDL_GLContext context) {
    _window = window;
    _context = context;

    _camera = std::make_unique<Camera>();
    _eventSystem = std::make_unique<EventSystem>();
    _footprint = std::make_unique<Footprint>(this);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForOpenGL(_window, _context);
    ImGui_ImplOpenGL3_Init();

    RegisterCallbacks();

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
    auto mesh = Mesh::Box();
    auto shader = Shader("assets/shaders/vert.vert", "assets/shaders/frag.frag");
    auto texture = Texture::LoadTexture("assets/penguin.jpg");
    shader.SetTexture2D(GL_TEXTURE0, texture);
    auto penguin = SceneObject(mesh, shader);

    // Grid setup
    auto planeMesh = Mesh::Plane(40.0f);
    auto gridShader = Shader("assets/shaders/vert.vert", "assets/shaders/grid.frag");
    auto plane = SceneObject(planeMesh, gridShader);

    _running = true;
    while (_running) {
        _eventSystem->ProcessEvents();

        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto pos = PolarToWorld(_cameraOrbitPosition);
        _camera->SetPosition(pos);
        _camera->LookAt(glm::vec3(0, 0, 0));

        _footprint->Draw(*_camera);
        plane.Draw(*_camera);
        penguin.Draw(*_camera);

        auto error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cout << "ERROR: " << error << std::endl;
        }

        DrawUI();
        SDL_GL_SwapWindow(_window);
    }

    SDL_GL_DestroyContext(_context);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Editor::RegisterEvent(Uint32 event, std::function<bool(SDL_Event *)> callback) {
    _eventSystem->RegisterEvent(event, callback);
}