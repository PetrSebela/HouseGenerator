#pragma once
#include <memory>

#include "camera.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "glm/vec3.hpp"

/**
 * Draggable handle
 */
class Handle {
    glm::vec3 _position = glm::vec3(0.0f);          // Handle position
    std::shared_ptr<Mesh> _mesh = Mesh::Plane(1.0f);  // Handle visualization mesh
    Shader _shader = Shader("assets/shaders/default.vert", "assets/shaders/handle.frag");   // Handle shader

public:
    Handle() = default;
    Handle(glm::vec3 position) : _position(position + glm::vec3(0.0f, 0.01f, 0.0f)) {};
    ~Handle() = default;

    /**
     * Computes distance from center of the handle
     * @param from Point from which the distance is measured
     * @return Distance from point
     */
    float Distance(glm::vec3 from);

    /**
     * Updates handle position
     * @param position New handle position in world coordinates
     */
    void SetPosition(glm::vec3 position);

    /**
     * Returns handle position in world coordinates
     * @return Handle position in world coordinates
     */
    glm::vec3 GetPosition();

    /**
     * Draws handle using provided camera
     */
    void Draw(Camera camera);
};