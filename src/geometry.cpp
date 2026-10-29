#include "geometry.hpp"

#include <stdexcept>

#include "line.hpp"

glm::vec3 geo::ClosestPointOnLine(glm::vec3 a, glm::vec3 b, glm::vec3 point) {
    auto segmentLength = distance(a, b);
    auto cursorVector = point - a;
    auto lineVector = (b - a) / segmentLength;
    float distance = glm::dot(cursorVector, lineVector);
    if (distance <= 0)
        return a;
    if (distance >= segmentLength)
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
    parallel = false;
}

void geo::Intersection(Segment a, Segment b, bool &parallel, glm::vec2 &point) {
    return Intersection(a.GetStartFlat(), a.GetDirectionFlat(), b.GetStartFlat(), b.GetDirectionFlat(), parallel,
                        point);
}

std::vector<Segment> geo::Raise(std::vector<Segment> &segments, float amount) {
    std::vector<Segment> offseted;
    for (auto segment: segments) {
        Segment offset(segment.Start + glm::vec3(0, amount, 0), segment.End + glm::vec3(0, amount, 0));
        offseted.push_back(offset);
    }
    return offseted;
}

std::vector<Segment> geo::Inset(std::vector<Segment> &segments, float amount) {
    std::vector<Segment> offseted;
    for (auto &segment: segments) {
        offseted.push_back(segment.Offset(amount));
    }
    return offseted;
}

std::vector<Segment> geo::ConnectNeighbours(std::vector<Segment> &segments, bool loop) {
    std::vector<Segment> connected;

    for (int i = 0; i < segments.size(); i++) {
        auto before = segments[(i + segments.size() - 1) % segments.size()];
        auto current = segments[i];
        auto after = segments[(i + 1) % segments.size()];

        bool parallel;

        glm::vec2 start = current.GetStartFlat();
        if (i != 0 || loop)
            Intersection(current, before, parallel, start);

        glm::vec2 end = current.GetEndFlat();
        if (i != segments.size() - 1 || loop)
            Intersection(current, after, parallel, end);

        Segment connectedSegment(glm::vec3(start.x, current.Start.y, start.y), glm::vec3(end.x, current.End.y, end.y));
        connected.push_back(connectedSegment);
    }

    return connected;
}

std::shared_ptr<Mesh> geo::BridgeSegments(std::vector<Segment> &a, std::vector<Segment> &b) {
    if (a.size() != b.size())
        throw std::invalid_argument("Bridge operation requires same amount of segments on each side");

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    for (int i = 0; i < a.size(); i++) {
        auto left = a[i];
        auto right = b[i];

        glm::vec3 normal = glm::cross(left.Direction(), glm::normalize(left.Start - right.Start));

        vertices.push_back({
            .position = left.Start,
            .normal = normal,
            .UV = glm::vec2(0, 0)
        });
        vertices.push_back({
            .position = left.End,
            .normal = normal,
            .UV = glm::vec2(left.Length(), 0)
        });
        vertices.push_back({
            .position = right.Start,
            .normal = normal,
            .UV = glm::vec2(0, glm::distance(left.Start, right.Start))
        });
        vertices.push_back({
            .position = right.End,
            .normal = normal,
            .UV = glm::vec2(right.Length(), glm::distance(left.End, right.End))
        });

        int base = i * 4;
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 2);
        indices.push_back(base + 1);
        indices.push_back(base + 3);
    }

    return std::make_shared<Mesh>(vertices, indices);
}
