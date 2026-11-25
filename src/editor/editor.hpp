#pragma once

#include <memory>
#include <SDL3/SDL.h>
#include "renderer/camera.hpp"
#include "event_system.hpp"
#include "footprint.hpp"
#include "generator/generator.hpp"
#include "renderer/scene_object.hpp"

enum RenderPass {
    Light,
    Geometry,
    PostProcess,
};

enum BufferView {
    Composite,
    Normal,
    Position,
    Depth,
    ShadowMap,
    PostProcessMask,
};

/**
 * Class representing editor functionality
 * Contains main event and render loops
 * Provides functionality regarding manipulation of generator data
 */
class Editor {
    SDL_Window* _window;    // SDL window pointer
    SDL_GLContext _context; // SDL context pointer

    std::shared_ptr<Camera> _camera;            // Main camera
    std::shared_ptr<Camera> _shadowCamera;      // Camera used for shadow mapping
    std::shared_ptr<EventSystem> _eventSystem;  // Event system reference
    std::shared_ptr<Footprint> _footprint;      // Footprint editor reference
    std::shared_ptr<Generator> _generator;      // House generator himself

    // Deferred rendering stuff
    GLuint _gBuffer;            // geometry frame buffer
    GLuint _depthBuffer;        // Depth buffer
    GLuint _albedoTexture;      // Color buffer
    GLuint _normalTexture;      // Normal buffer
    GLuint _positionTexture;    // Position buffer (in world space)
    GLuint _postProcessMask;    // Position buffer (in world space)

    // Shadow mapping buffers
    GLuint _shadowMapFBO;       // Shadow mapping frame buffer
    GLuint _shadowMapTexture;   // Shadow map depth texture
    static constexpr int SHADOW_MAP_RESOLUTION = 16000;  // Shadow map resolution

    // Editor flags and other data
    int _windowWidth;           // Window width
    int _windowHeight;          // Window height
    bool _running = false;      // If editor is running
    bool _cameraPan = false;    // If camera is panning (lmb down)
    bool _wireframe = false;    // Show wireframe of generated model
    double _lastFrameTime;      // Time at start of last frame

    // Generator settings flags
    bool _mirrorRoof = true;

    glm::vec3 _cameraOrbitPosition = glm::vec3(0.0f, -10.0f, 15.0f);
    glm::vec3 _cameraCenter = glm::vec3(0.0f, 0.5f, 0.0f);

    BufferView _bufferView = Composite;
    RenderPass _renderPass = RenderPass::Light;

    std::vector<float> _frametimes;

    inline static std::shared_ptr<Editor> _instance;

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
    ~Editor() = default; // No need to free the opengl buffers, the OS will take care of them. Right? (・・;)

    static std::shared_ptr<Editor> InitSingleton(SDL_Window* window, SDL_GLContext context);
    static std::shared_ptr<Editor> GetInstance();

    RenderPass GetRenderPass();

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

    void RenderScreeQuad(std::shared_ptr<Mesh> quad, std::shared_ptr<Shader> postProcessShader);

    void RenderScene(std::shared_ptr<Mesh> skybox, std::shared_ptr<Shader> skyboxShader, SceneObject grid);

    void RenderLightmap(SceneObject grid, std::shared_ptr<Shader> lightShader);

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

