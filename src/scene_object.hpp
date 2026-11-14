#pragma once

#include "mesh.hpp"
#include "shader.hpp"
#include "camera.hpp"

/**
 * Represents object in scene
 */
class SceneObject {
    std::shared_ptr<Mesh> _mesh;
    std::shared_ptr<Shader> _shader;
    glm::mat4x4 _modelMatrix = glm::identity<glm::mat4x4>();
    glm::vec3 _position = glm::vec3(0,0,0);
    glm::vec3 _rotation = glm::vec3(0,0,0);
    glm::vec3 _scale = glm::vec3(1.0f);
    void UpdateModelMatrix();

public:
    SceneObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader);
    ~SceneObject() = default;
    void SetMesh(std::shared_ptr<Mesh> mesh);
    void SetShader(std::shared_ptr<Shader> shader);
    void SetPosition(glm::vec3 rotation);
    void SetRotation(glm::vec3 rotation);
    void SetScale(glm::vec3 scale);
    void Draw();
};