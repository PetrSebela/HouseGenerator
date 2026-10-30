#pragma once

#include <memory>
#include <SDL3/SDL.h>
#include "camera.hpp"
#include "event_system.hpp"
#include "footprint.hpp"

/**
 * Class representing editor functionality
 * Contains main event and render loops
 * Provides functionality regarding manipulation of generator data
 */
class Editor {
    SDL_Window* _window;    // SDL window pointer
    SDL_GLContext _context; // SDL context pointer

    std::unique_ptr<Camera> _camera;            // Main camera
    std::unique_ptr<Camera> _shadowCamera;      // Camera used for shadow mapping
    std::unique_ptr<EventSystem> _eventSystem;  // Event system reference
    std::unique_ptr<Footprint> _footprint;      // Footprint editor reference

    // Deferred rendering stuff
    GLuint _gBuffer;            // geometry frame buffer
    GLuint _depthBuffer;        // Depth buffer
    GLuint _albedoTexture;      // Color buffer
    GLuint _normalTexture;      // Normal buffer
    GLuint _positionTexture;    // Position buffer (in world space)

    // Shadow mapping buffers
    GLuint _shadowMapFBO;       // Shadow mapping frame buffer
    GLuint _shadowMapTexture;   // Shadow map depth texture
    static constexpr int SHADOW_MAP_RESOLUTION = 4096;  // Shadow map resolution

    // Editor flags and other data
    int _windowWidth;           // Window width
    int _windowHeight;          // Window height
    bool _running = false;      // If editor is running
    bool _cameraPan = false;    // If camera is panning (lmb down)
    double _lastFrameTime;      // Time at start of last frame
    glm::vec3 _cameraOrbitPosition = glm::vec3(0.0f, -10.0f, 15.0f);
    glm::vec3 _cameraCenter = glm::vec3(0.0f, 0.5f, 0.0f);
    bool _wireframe = false;    // Show wireframe of generated model

    /**
     * Registers editor callback with event system (unregistration is handled by deleting event system ┐(´～`)┌ )
     */
    void RegisterCallbacks();

    /**
     * Draws editor UI using DearImGui
     */
    void DrawUI();

    /**
     * Creates opengl buffers
     */
    void SetupBuffers();

public:
    /**
     * @return Cursor world position on camera near clip plane
     */
    glm::vec3 GetCursorWorldPosition();

    Editor(SDL_Window* window, SDL_GLContext context);
    Editor() = default;
    ~Editor() = default; // No need to free the opengl buffers, the OS will take care of them right(・・;)?

    /**
     * Converts polar coordinates (pitch, yaw, distance) to world space
     * @param polar polar coordinates (pitch, yaw, distance)
     * @return coordinates in world position
     */
    static glm::vec3 PolarToWorld(glm::vec3 polar);

    /**
     * @return Cursor position in NDC
     */
    glm::vec2 GetCursorNDC();

    /**
     * Runs the editor
     */
    void Run();

    /**
     * Registers callback with main event system
     * @param event SDL event type
     * @param callback Event callback function
     */
    void RegisterEvent(Uint32 event, std::function<bool(SDL_Event*)> callback);
};

