#include "geometry.hpp"

glm::vec3 geo::ClosestPointOnLine(glm::vec3 a, glm::vec3 b, glm::vec3 point) {
    auto segmentLength = distance(a, b);
    auto cursorVector = point - a;
    auto lineVector = (b - a) / segmentLength;
    float distance = glm::dot(cursorVector, lineVector);
    if(distance <= 0)
        return a;
    if(distance >= segmentLength)
        return b;

    return a + lineVector * distance;
}

float geo::DistanceToLine(glm::vec3 a, glm::vec3 b, glm::vec3 point) {
    auto closest = ClosestPointOnLine(a, b, point);
    return glm::distance(closest, point);
}
