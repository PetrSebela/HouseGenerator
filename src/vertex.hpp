#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <glad/gl.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 UV;
};

#endif
