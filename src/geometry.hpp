#pragma once
#include "glm/glm.hpp"

namespace geo {
    glm::vec3 ClosestPointOnLine(glm::vec3 a, glm::vec3 b, glm::vec3 point);
    float DistanceToLine(glm::vec3 a, glm::vec3 b, glm::vec3 point);
}
