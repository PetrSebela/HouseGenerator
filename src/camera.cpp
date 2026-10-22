#include "camera.hpp"

#include "glm/gtc/quaternion.hpp"
#include "glad/gl.h"
#include "SDL3/SDL_video.h"

Camera::Camera(bool orthographic) {
    _isOrthographic = orthographic;
}

void Camera::SetPosition(glm::vec3 position) {
    _position = position;
}

void Camera::LookAt(glm::vec3 worldPosition) {
    _lookAt = worldPosition;
}

void Camera::SetAspectRatio(double aspectRatio) {
    _aspectRatio = aspectRatio;
}

glm::vec3 Camera::ScreenToWorld(glm::vec2 screenPositionNormalized) {
    auto cameraMatrix = GetViewMatrix();
    auto world = glm::inverse(cameraMatrix) * glm::vec4(screenPositionNormalized,-1.0f, 1.0f);
    return glm::vec3(world) / world.w;
}

glm::vec3 Camera::ScreenToDirection(glm::vec2 screenPositionNormalized) {
    auto cameraMatrix = GetViewMatrix();
    auto world = glm::inverse(cameraMatrix) * glm::vec4(screenPositionNormalized,-1.0f, 1.0f);
    return glm::normalize(glm::vec3(world / world.w) - _position);
}

glm::mat4x4 Camera::GetViewMatrix() {
    glm::mat4x4 projection;

    if (_isOrthographic)
        projection = glm::ortho(-20.0f,20.0f,-20.0f,20.0f,0.0f,75.0f);
    else
        projection = glm::perspective(glm::radians(_fov / 2.0), _aspectRatio, 0.1, 100.0);

    auto look = glm::lookAt(_position, _lookAt, glm::vec3(0.0,1.0,0.0));
    return projection * look;
}

glm::vec3 Camera::GetPosition() {
    return _position;
}
