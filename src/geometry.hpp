#pragma once
#include "glm/glm.hpp"
#include "segment.hpp"

namespace geo {
    glm::vec3 ClosestPointOnLine(glm::vec3 a, glm::vec3 b, glm::vec3 point);
    float DistanceToLine(glm::vec3 a, glm::vec3 b, glm::vec3 point);

    void Intersection(glm::vec2 a, glm::vec2 adx, glm::vec2 b, glm::vec2 bdx, bool &parallel, glm::vec2 &point);
    std::vector<glm::vec3> Inset(std::vector<glm::vec3>& points, float amount);

    std::vector<Segment> NeighbourIntersect(std::vector<Segment>& segments, bool loop);
}
