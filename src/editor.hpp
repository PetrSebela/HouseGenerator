#ifndef _EDITOR_H_
#define _EDITOR_H_
#include <SDL3/SDL.h>
#include "camera.hpp"
#include "event_system.hpp"

class Editor {
    SDL_Window* _window;
    SDL_GLContext _context;
    Camera _camera;
    EventSystem _eventSystem;

    bool _running = false;
    bool _cameraPan = false;
public:
    Editor();
    ~Editor() = default;
    static glm::vec3 PolarToWorld(glm::vec3 polar);
    glm::vec2 GetCursorNDC();
    void Run();

    // These methods are temporarily here, until I implement the class where they should actually be
    bool PointInRange(glm::vec3 cursor, std::vector<glm::vec3> points, int* targetPoint);
    glm::vec3 GetPointOnPlane();
};
#endif
