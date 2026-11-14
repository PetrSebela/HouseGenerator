#include "scene_object.hpp"

#include <iostream>
#include <ostream>

#include "camera.hpp"

void SceneObject::UpdateModelMatrix() {
    _modelMatrix = glm::identity<glm::mat4>();
    _modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    _modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    _modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    _modelMatrix = glm::translate(_modelMatrix, _position);
    _modelMatrix = glm::scale(_modelMatrix, _scale);
}

SceneObject::SceneObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader) {
    _mesh = mesh;
    _shader = shader;
    UpdateModelMatrix();
}

void SceneObject::SetMesh(std::shared_ptr<Mesh> mesh) {
    _mesh = mesh;
}

void SceneObject::SetShader(std::shared_ptr<Shader> shader) {
    _shader = shader;
}

void SceneObject::SetPosition(glm::vec3 position) {
    _position = position;
    UpdateModelMatrix();
}

void SceneObject::SetRotation(glm::vec3 rotation) {
    _rotation = rotation;
    UpdateModelMatrix();
}

void SceneObject::SetScale(glm::vec3 scale) {
    _scale = scale;
    UpdateModelMatrix();
}

void SceneObject::Draw() {
    _shader->Bind();
    _shader->SetMatrix4x4("model", _modelMatrix);
    _mesh->Draw();
}
