#pragma once
#include "camera.hpp"
#include "shader.hpp"
#include "mesh.hpp"

/**
 * Object representing line
 * TODO: add line width parametes
 * TODO: user provided shaders
 */
class Line {
    std::shared_ptr<Mesh> _mesh = std::make_shared<Mesh>(); // Line mesh
    Shader _shader = Shader("assets/shaders/default.vert","assets/shaders/color.frag"); // Line shader
    bool _isLoop = false;   // If line should form a loop
public:
    Line() = default;
    Line(std::vector<glm::vec3> points);
    ~Line();

    /**
     * Set line loop
     * TODO: move to constructor
     */
    void SetLoop(bool loop);

    /**
     * Draw line using provided camera
     */
    void Draw(Camera camera);

    /**
     * Set line points
     */
    void SetPoints(std::vector<glm::vec3> points);
};