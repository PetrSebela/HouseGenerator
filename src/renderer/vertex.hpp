#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

/**
 * Global mesh vertex format
 *
 * BE AWARE THAT UVs are bound to layout=4 at default and post process vertex shaders
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 UV;
};
