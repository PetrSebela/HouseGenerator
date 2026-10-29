#pragma once
#include "mesh.hpp"
#include "glm/glm.hpp"
#include "segment.hpp"
#include "vertex.hpp"

namespace geo {
    glm::vec3 ClosestPointOnLine(glm::vec3 a, glm::vec3 b, glm::vec3 point);
    float DistanceToLine(glm::vec3 a, glm::vec3 b, glm::vec3 point);

    void Intersection(glm::vec2 a, glm::vec2 adx, glm::vec2 b, glm::vec2 bdx, bool &parallel, glm::vec2 &point);
    void Intersection(Segment a, Segment b, bool &parallel, glm::vec2 &point);
    std::vector<Segment> Raise(std::vector<Segment> &segments, float amount);
    std::vector<Segment> Inset(std::vector<Segment>& segments, float amount);
    std::vector<Segment> ConnectNeighbours(std::vector<Segment>& segments, bool loop);
    std::shared_ptr<Mesh> BridgeSegments(std::vector<Segment>& a, std::vector<Segment>& b);
}
