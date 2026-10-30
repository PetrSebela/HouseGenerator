#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

/**
 * @brief Class representing camera and providing useful functionality regarding camera
 */
class Camera {
    glm::vec3 _position;            // Camera position in world coordinates
    glm::vec3 _lookAt;              // Point (in world coordinates) at which the camera look at all times
    float _fov = 90;                // Camera FOV in degrees
    double _aspectRatio = 1.7778f;  // Camera aspect ratio ( 16:9 by default )
    bool _isOrthographic = false;   // If camera uses orthographic projection ( in ideal world, i would use polymorphism to differentiate camera types)

public:
    Camera() = default;
    Camera(bool orthographic);
    ~Camera() = default;

    /**
     * Updates camera position (in world coordinates)
     */
    void SetPosition(glm::vec3 position);

    /**
     * Sets point at which the camera look at (in world coordinates)
     */
    void LookAt(glm::vec3 worldPosition);

    /**
     * Sets camera aspect ratio
     */
    void SetAspectRatio(double aspectRatio);

    /**
     * Converts screen position (NDC) to world position on camera near plane
     * @bug does not account for orthographic camera
     * @param screenPosition screen position in NDC
     * @return world position on camera near plane
     */
    glm::vec3 ScreenToWorld(glm::vec2 screenPosition);

    /**
     * Converts screen position (NDC) to direction vector between camera center and point corresponding to NDC on camera near plane
     * @bug does not account for orthographic camera
     * @param screenPositionNormalized screen position in NDC
     * @return ray direction
     */
    glm::vec3 ScreenToDirection(glm::vec2 screenPositionNormalized);

    /**
     * Computes and returns camera view-projection matrix
     * @return Camera view-projection matrix
     */
    glm::mat4x4 GetMatrix();

    /**
     * @return Camera position in world space
     */
    glm::vec3 GetPosition();
};