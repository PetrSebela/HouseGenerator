#include "camera.hpp"

#include "glm/gtc/quaternion.hpp"

void Camera::SetPosition(glm::vec3 position) {
    _position = position;
}

void Camera::SetLookDirection(glm::vec3 lookDirection) {
    _rotation = glm::quatLookAt(lookDirection, glm::vec3(0,1,0));
}

void Camera::LookAt(glm::vec3 worldPosition) {
    _rotation = glm::quatLookAt(worldPosition - _position, glm::vec3(0,1,0));
}

void Camera::SetAspectRatio(double aspectRatio) {
    _aspectRatio = aspectRatio;
}

glm::mat4x4 Camera::GetViewMatrix() {
    auto translation = glm::translate(glm::identity<glm::mat4x4>(), _position);
    auto projection = glm::perspective(glm::radians(_fov / 2.0), _aspectRatio, 0.1, 100.0);
    auto look = glm::lookAt(_position, glm::vec3(0,0,0), glm::vec3(0.0,1.0,0.0));
    return glm::mat4x4(projection) * look * glm::inverse(translation) ;
}
