#include "editor.hpp"
#include <glad/gl.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <iostream>

#include "../../external/imgui/backends/imgui_impl_sdl3.h"
#include "../../external/imgui/backends/imgui_impl_opengl3.h"

#include "renderer/shader.hpp"
#include "texture.hpp"
#include "renderer/mesh.hpp"
#include "scene_object.hpp"
#include "event_system.hpp"
#include "gizmos.hpp"
#include "generator/generator.hpp"

void Editor::RegisterCallbacks() {
    _eventSystem->RegisterEvent(SDL_EVENT_QUIT, [this](auto &&) { _running = false; return false; });

    _eventSystem->RegisterEvent(SDL_EVENT_KEY_DOWN, [this](const SDL_Event *event) {
        if (event->key.key == SDLK_ESCAPE)
            _running = false;
        return false;
    });

    _eventSystem->RegisterEvent(SDL_EVENT_KEY_DOWN, [this](const SDL_Event *event) {
        if (event->key.key == SDLK_W)
            _wireframe = !_wireframe;
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
        _cameraOrbitPosition.z = glm::clamp(_cameraOrbitPosition.z, 1.f, 75.f);
        return false;
    });

    _eventSystem->RegisterEvent(SDL_EVENT_MOUSE_MOTION, [this](const SDL_Event *event) {
        if (!_cameraPan)
            return false;
        _cameraOrbitPosition -= glm::vec3(event->motion.xrel / 10.0, event->motion.yrel / 10.0, 0);
        // _cameraOrbitPosition.y = glm::clamp(_cameraOrbitPosition.y, -90.f + 1, -0.1f);
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
    ImGui::Checkbox("Use wireframe", &_wireframe);
    ImGui::Checkbox("Relax selected", &_footprint->Relax);
    ImGui::Checkbox("Snap to grid", &_footprint->SnapToGrid);
    ImGui::SliderFloat("Grid size", &_footprint->GridSize, 0.01, 1.0f);

    const char* items[] = {"Composite", "Albedo", "Position", "Normal", "Depth", "Shadow", "PP Target"};
    ImGui::Combo("Buffer view", (int*)&_bufferView, items, 7);

    ImGui::End();

    // ImGui::ShowDemoWindow();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::SetupBuffers() {
    glGenFramebuffers(1, &_gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

    _albedoTexture = Texture::GenTexture(4096, 2160);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _albedoTexture, 0);

    _normalTexture = Texture::GenTexture(4096, 2160);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _normalTexture, 0);

    _positionTexture = Texture::GenTexture(4096, 2160);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _positionTexture, 0);

    _postProcessMask = Texture::GenTexture(4096, 2160);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _postProcessMask, 0);

    GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, buffers);

    glGenTextures(1, &_depthBuffer);
    glBindTexture(GL_TEXTURE_2D, _depthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 4096, 2160, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthBuffer, 0);

    // Shadows
    glGenFramebuffers(1, &_shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _shadowMapFBO);

    glGenTextures(1, &_shadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, _shadowMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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

    _camera = std::make_shared<Camera>();
    _shadowCamera = std::make_shared<Camera>(true);
    _eventSystem = std::make_shared<EventSystem>();
    _footprint = std::make_shared<Footprint>(this);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForOpenGL(_window, _context);
    ImGui_ImplOpenGL3_Init();

    RegisterCallbacks();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    glLineWidth(2.0f);

    SetupBuffers();
}

std::shared_ptr<Editor> Editor::InitSingleton(SDL_Window *window, SDL_GLContext context) {
    if (_instance != nullptr) {
        throw std::runtime_error("Editor singleton is already initialized");
    }
    _instance = std::make_shared<Editor>(window, context);
    return _instance;
}

std::shared_ptr<Editor> Editor::GetInstance() {
    return _instance;
}

RenderPass Editor::GetRenderPass() {
    return _renderPass;
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

void Editor::RenderScreeQuad(std::shared_ptr<Mesh> quad, std::shared_ptr<Shader> postProcessShader) {
    _renderPass = PostProcess;
    glEnable(GL_FRAMEBUFFER_SRGB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, _windowWidth, _windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    postProcessShader->Bind();
    postProcessShader->SetInt("bufferView", _bufferView);
    quad->Draw();
    glDisable(GL_FRAMEBUFFER_SRGB);
}

void Editor::RenderScene(std::shared_ptr<Mesh> skybox, std::shared_ptr<Shader> skyboxShader, SceneObject grid, Generator generator) {
    _renderPass = Geometry;
    glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
    glViewport(0, 0, 4096, 2160);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render skybox
    glDepthMask(GL_FALSE);
    skyboxShader->Bind();
    skyboxShader->SetMatrix4x4("view", _camera->GetProjectionMatrix());
    skybox->Draw();
    glDepthMask(GL_TRUE);

    grid.Draw();

    glPolygonMode(GL_FRONT_AND_BACK, _wireframe ? GL_LINE : GL_FILL);
    generator.Draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    _footprint->Draw();

    Gizmos::GetInstance()->DrawGizmos(_camera);
}

void Editor::RenderLightmap(SceneObject grid, std::shared_ptr<Shader> lightShader, Generator generator) {
    _renderPass = Light;
    glBindFramebuffer(GL_FRAMEBUFFER, _shadowMapFBO);
    glViewport(0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
    glClear(GL_DEPTH_BUFFER_BIT);

    lightShader->Bind();
    lightShader->SetMatrix4x4("model", glm::identity<glm::mat4>());
    if (!_wireframe)
        generator.Draw();
    grid.Draw();
}

void Editor::Run() {
    auto skyboxTexture = Texture::LoadSkybox("assets/textures/skybox");

    // Skybox setup
    auto skybox = Mesh::Cube();
    auto skyboxShader = Shader::LoadShader("assets/shaders/default.vert", "assets/shaders/skybox.frag");
    skyboxShader->SetTexture2D(GL_TEXTURE0, skyboxTexture);

    // Grid setup
    auto floor = Mesh::Plane(150.0f);
    auto floorShader = Shader::LoadShader("assets/shaders/default.vert", "assets/shaders/floor.frag");
    auto grid = SceneObject(floor, floorShader);
    _shadowCamera->SetPosition(glm::vec3(9,12,15));
    _shadowCamera->LookAt(glm::vec3(0,0,0));

    SDL_GetWindowSize(_window, &_windowWidth, &_windowHeight);

    auto quad = Mesh::Quad();
    auto lightShader = Shader::LoadShader("assets/shaders/shadow.vert", "assets/shaders/empty.frag");
    auto postProcessShader = Shader::LoadShader("assets/shaders/post_process.vert", "assets/shaders/post_process.frag");
    postProcessShader->SetTexture2D(GL_TEXTURE0, _albedoTexture);
    postProcessShader->SetTexture2D(GL_TEXTURE1, _normalTexture);
    postProcessShader->SetTexture2D(GL_TEXTURE2, _positionTexture);
    postProcessShader->SetTexture2D(GL_TEXTURE3, _depthBuffer);
    postProcessShader->SetTexture2D(GL_TEXTURE4, _shadowMapTexture);
    postProcessShader->SetTexture2D(GL_TEXTURE5, _postProcessMask);
    Generator generator;

    _running = true;
    while (_running) {
        _eventSystem->ProcessEvents();
        generator.SetFootprint(_footprint->GetOutline());

        auto pos = PolarToWorld(_cameraOrbitPosition) + _cameraCenter;
        _camera->SetPosition(pos);
        _camera->LookAt(_cameraCenter);

        Shader::SetGlobalMatrix4x4("light", _shadowCamera->GetMatrix());
        Shader::SetGlobalMatrix4x4("view", _camera->GetMatrix());

        RenderLightmap(grid, lightShader, generator);
        RenderScene(skybox, skyboxShader, grid, generator);
        RenderScreeQuad(quad, postProcessShader);

        DrawUI();
        SDL_GL_SwapWindow(_window);

        // Limit framerate so my poor GPU does not explode
        SDL_Delay(10);
    }

    SDL_GL_DestroyContext(_context);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Editor::RegisterEvent(Uint32 event, std::function<bool(SDL_Event *)> callback) {
    _eventSystem->RegisterEvent(event, callback);
}