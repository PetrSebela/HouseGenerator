#pragma once
#include <memory>
#include <SDL3/SDL.h>
#include "camera.hpp"
#include "event_system.hpp"
#include "handle.hpp"
#include "footprint.hpp"

class Editor {
    SDL_Window* _window;
    SDL_GLContext _context;

    std::unique_ptr<Camera> _camera;
    std::unique_ptr<EventSystem> _eventSystem;
    std::unique_ptr<Footprint> _footprint;

    bool _running = false;
    bool _cameraPan = false;
    double _lastFrameTime;

    glm::vec3 _cameraOrbitPosition = glm::vec3(0.0f, 0.0f, 5.0f);

    void RegisterCallbacks();
    void DrawUI();

public:
    glm::vec3 GetCursorWorldPosition();
    Editor(SDL_Window* window, SDL_GLContext context);
    Editor() = default;
    ~Editor() = default;
    static glm::vec3 PolarToWorld(glm::vec3 polar);
    glm::vec2 GetCursorNDC();
    void Run();
    void RegisterEvent(Uint32 event, std::function<bool(SDL_Event*)> callback);
};

