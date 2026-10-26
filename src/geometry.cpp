#include "geometry.hpp"

#include "line.hpp"

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

void geo::Intersection(glm::vec2 a, glm::vec2 adx, glm::vec2 b, glm::vec2 bdx, bool &parallel, glm::vec2 &point) {
    float cross = adx.x * bdx.y - adx.y * bdx.x;

    if (cross == 0.0f) {
        parallel = true;
        return;
    }

    glm::vec2 diff = b - a;
    float t = (diff.x * bdx.y - diff.y * bdx.x) / cross;

    point = a + adx * t;
    parallel = false;;
}
