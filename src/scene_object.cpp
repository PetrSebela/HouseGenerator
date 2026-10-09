#include "scene_object.hpp"
#include "camera.hpp"

void SceneObject::UpdateModelMatrix() {
    _modelMatrix = glm::identity<glm::mat4>();
    _modelMatrix = glm::scale(_modelMatrix, _scale);
    _modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    _modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    _modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    _modelMatrix = glm::translate(_modelMatrix, _position);
}

SceneObject::SceneObject(Mesh mesh, Shader shader) {
    _mesh = mesh;
    _shader = shader;
    UpdateModelMatrix();
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

void SceneObject::Draw(Camera camera) {
    _shader.Bind();
    // _shader.SetMatrix4x4("model", _modelMatrix);
    _shader.SetMatrix4x4("model", glm::identity<glm::mat4x4>());


    auto c = glm::inverse(glm::translate(glm::identity<glm::mat4x4>(), glm::vec3(0,0,5.0f)));
    // shader.SetMatrix4x4("camera", camera);

    glm::mat4 projection_matrix = glm::perspective(glm::radians(60 / 2.0), 16.0 / 9.0, 0.1, 100.0);
    // shader.SetMatrix4x4("view", projection_matrix * camera);
    auto cameraMatrix = camera.GetViewMatrix();
    _shader.SetMatrix4x4("view", cameraMatrix);
    // _shader.SetMatrix4x4("view", projection_matrix * c);

    _mesh.Draw();
}
