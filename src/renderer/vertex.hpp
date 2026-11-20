#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

/**
 * Global mesh vertex format
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 UV;
};
