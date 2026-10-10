#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
    glm::vec3 _position;
    glm::quat _rotation;
    float _fov = 90;
    double _aspectRatio = 16.0f/9.0f;
public:
    Camera() = default;
    ~Camera() = default;
    void SetPosition(glm::vec3 position);
    void SetLookDirection(glm::vec3 lookDirection);
    void LookAt(glm::vec3 worldPosition);
    void SetAspectRatio(double aspectRatio);
    glm::mat4x4 GetViewMatrix();
};


#endif
