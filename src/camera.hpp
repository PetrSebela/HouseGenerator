#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
    glm::vec3 _position;
    glm::vec3 _lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
    float _fov = 90;
    double _aspectRatio = 16.0f/9.0f;
    bool _isOrthographic = false;

public:
    Camera() = default;
    Camera(bool orthographic);
    ~Camera() = default;
    void SetPosition(glm::vec3 position);
    void LookAt(glm::vec3 worldPosition);
    void SetAspectRatio(double aspectRatio);
    glm::vec3 ScreenToWorld(glm::vec2 screenPosition);
    glm::vec3 ScreenToDirection(glm::vec2 screenPositionNormalized);
    glm::mat4x4 GetViewMatrix();
    glm::vec3 GetPosition();
};

#endif
