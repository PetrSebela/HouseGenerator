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
#include "generator.hpp"

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
        SDL_GetWindowSize(_window, &_windowWidth, &_windowHeight);
        _camera->SetAspectRatio((float) _windowWidth / (float) _windowHeight);
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

void Editor::SetupBuffers() {
    glGenFramebuffers(1, &_defaultFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);

    glGenTextures(1, &_renderTexture);
    glBindTexture(GL_TEXTURE_2D, _renderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4096, 2160, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderTexture, 0);

    glGenRenderbuffers(1, &_renderBufferObject);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderBufferObject);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 4096, 2160);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _renderBufferObject);

    // Shadows
    glGenFramebuffers(1, &_shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _shadowMapFBO);

    glGenTextures(1, &_shadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, _shadowMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowMapTexture, 0);
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
    _shadowCamera = std::make_unique<Camera>(true);
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

    SetupBuffers();
}

glm::vec3 Editor::PolarToWorld(glm::vec3 polar) {
    auto horizontal = glm::angleAxis(glm::radians(polar.x), glm::vec3(0, 1, 0));
    auto vertical = glm::angleAxis(glm::radians(polar.y), glm::vec3(1, 0, 0));
    return horizontal * vertical * glm::vec3(0, 0, polar.z);
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
    auto penguinBox = Mesh::Box();
    auto penguinShader = Shader("assets/shaders/vert.vert", "assets/shaders/frag.frag");
    auto penguinTexture = Texture::LoadTexture("assets/penguin.jpg");
    auto penguin = SceneObject(penguinBox, penguinShader);

    auto brickAlbedo = Texture::LoadTexture("assets/textures/bricks/albedo.jpg");
    auto brickNormal = Texture::LoadTexture("assets/textures/bricks/normal.jpg");

    // Grid setup
    auto planeMesh = Mesh::Plane(40.0f);
    auto gridShader = Shader("assets/shaders/vert.vert", "assets/shaders/grid.frag");
    auto grid = SceneObject(planeMesh, gridShader);

    Shader shadowShader("assets/shaders/vert.vert", "assets/shaders/shadow.frag");
    Shader wallShader("assets/shaders/vert.vert", "assets/shaders/wall.frag");

    // Camera shadowCamera(true);
    _shadowCamera->SetPosition(glm::vec3(9,12,15));
    _shadowCamera->LookAt(glm::vec3(0,0,0));

    SDL_GetWindowSize(_window, &_windowWidth, &_windowHeight);

    Mesh quad = Mesh::Quad();
    Shader lightShader("assets/shaders/shadow.vert", "assets/shaders/empty.frag");
    Shader postProcessShader("assets/shaders/post_process.vert", "assets/shaders/post_process.frag");
    Generator generator;

    _running = true;
    while (_running) {
        _eventSystem->ProcessEvents();

        // Shadow
        glBindFramebuffer(GL_FRAMEBUFFER, _shadowMapFBO);
        glViewport(0, 0, 2048, 2048);
        glClear(GL_DEPTH_BUFFER_BIT);
        lightShader.Bind();
        lightShader.SetMatrix4x4("lightSpaceTransform", _shadowCamera->GetViewMatrix());
        penguinBox.Draw();
        planeMesh.Draw();
        generator.SetFootprint(_footprint->GetOutline());
        generator.Draw(*_shadowCamera);

        glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBO);
        glViewport(0, 0, 4096, 2160);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        penguinShader.SetTexture2D(GL_TEXTURE0, penguinTexture);

        auto pos = PolarToWorld(_cameraOrbitPosition) + _cameraCenter;
        _camera->SetPosition(pos);
        _camera->LookAt(_cameraCenter);

        shadowShader.Bind();
        shadowShader.SetMatrix4x4("view", _camera->GetViewMatrix());
        shadowShader.SetMatrix4x4("light", _shadowCamera->GetViewMatrix());
        shadowShader.SetTexture2D(GL_TEXTURE0, _shadowMapTexture);
        planeMesh.Draw();

        wallShader.Bind();
        wallShader.SetMatrix4x4("view", _camera->GetViewMatrix());
        wallShader.SetMatrix4x4("light", _shadowCamera->GetViewMatrix());
        wallShader.SetTexture2D(GL_TEXTURE1, brickAlbedo);
        wallShader.SetTexture2D(GL_TEXTURE2, brickNormal);
        generator.SetFootprint(_footprint->GetOutline());
        generator.Draw(*_camera);

        penguinShader.SetTexture2D(GL_TEXTURE0, penguinTexture);
        penguin.Draw(*_camera);
        _footprint->Draw(*_camera);

        // Render frame to screen with post processing
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, _windowWidth, _windowHeight);
        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        postProcessShader.Bind();
        postProcessShader.SetTexture2D(GL_TEXTURE0, _renderTexture);
        // postProcessShader.SetTexture2D(GL_TEXTURE0, _shadowMapTexture);
        quad.Draw();

        glDisable(GL_FRAMEBUFFER_SRGB);
        DrawUI();
        glEnable(GL_FRAMEBUFFER_SRGB);
        SDL_GL_SwapWindow(_window);
    }

    SDL_GL_DestroyContext(_context);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Editor::RegisterEvent(Uint32 event, std::function<bool(SDL_Event *)> callback) {
    _eventSystem->RegisterEvent(event, callback);
}