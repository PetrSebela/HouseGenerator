#pragma once
#include "camera.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "glm/vec3.hpp"

// TODO: add variable size
// Some terribleness is happening here with the offsets, so does with lines
class Handle {
    glm::vec3 _position = glm::vec3(0.0f);
    Mesh _mesh = Mesh::Plane(1);
    Shader _shader = Shader("assets/shaders/vert.vert", "assets/shaders/handle.frag");

public:
    Handle() = default;
    Handle(glm::vec3 position) : _position(position + glm::vec3(0.0f, 0.01f, 0.0f)) {};
    ~Handle() = default;
    float Distance(glm::vec3 from);
    void SetPosition(glm::vec3 position);
    glm::vec3 GetPosition();
    void Draw(Camera camera);
};