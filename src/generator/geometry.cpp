#include "geometry.hpp"

#include <algorithm>
#include <iostream>
#include <ostream>
#include <stack>
#include <stdexcept>

#include "renderer/line.hpp"

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

    if (std::abs(cross) <= 1e-6f * glm::length(adx) * glm::length(bdx)) {
        parallel = true;
        return;
    }

    glm::vec2 diff = b - a;
    float t = (diff.x * bdx.y - diff.y * bdx.x) / cross;

    point = a + adx * t;
    parallel = false;
}

void geo::Intersection(Segment a, Segment b, bool &parallel, glm::vec2 &point) {
    return Intersection(a.GetStartFlat(),
                    a.GetFlatNormal(),
                    b.GetStartFlat(),
                    b.GetFlatNormal(),
                    parallel,
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

    return  connected;
}

std::shared_ptr<Mesh> geo::BridgeSegments(std::vector<Segment> &a, std::vector<Segment> &b) {
    if (a.size() != b.size())
        throw std::invalid_argument("Bridge operation requires same amount of segments on each side");

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    for (int i = 0; i < a.size(); i++) {
        auto left = a[i];
        auto right = b[i];

        glm::vec3 normal = glm::cross(left.GetDirection(), glm::normalize(left.Start - right.Start));

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

std::shared_ptr<Mesh> geo::WallsToMesh(std::vector<Wall> &walls) {
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    for (Wall &wall : walls) {
        mesh = Mesh::Join(mesh, wall.GetMesh());
    }

    return mesh;
}


std::shared_ptr<Mesh> geo::Fill(std::vector<Segment> outline) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    std::vector<glm::vec3> points;
    for (const auto& point: outline) {
        points.push_back(point.Start);
    }

    int offset = 0;
    while (points.size() >= 3 && offset < points.size()) {
        auto start = points[offset];
        auto middle = points[(offset + 1) % points.size()];
        auto end = points[(offset + 2) % points.size()];

        if (!IsClockwise(start, middle, end)) {
            offset++;
            continue;
        }

        bool invalid = false;
        for (int i = 0; i < points.size(); i++) {
            invalid |= IsInsideTriangle(Flatten(start), Flatten(middle), Flatten(end), Flatten(points[i]));
        }
        if (invalid) {
            offset++;
            continue;
        }

        points.erase(points.begin() + (offset + 1) % points.size());

        indices.push_back(vertices.size());
        indices.push_back(vertices.size() + 1);
        indices.push_back(vertices.size() + 2);

        vertices.push_back({start, glm::vec3(0), glm::vec3(0,1.0,0), glm::vec2(start.x, start.z)});
        vertices.push_back({middle, glm::vec3(0), glm::vec3(0,1.0,0), glm::vec2(middle.x, middle.z)});
        vertices.push_back({end, glm::vec3(0), glm::vec3(0,1.0,0), glm::vec2(end.x, end.z)});
        offset = 0;
    }
    return std::make_shared<Mesh>(vertices, indices);
}

bool geo::Intersects(std::vector<Segment> &segments, glm::vec3 a, glm::vec3 b) {
    glm::vec2 flatStart = glm::vec2(a.x, a.z);
    glm::vec2 flatEnd = glm::vec2(b.x, b.z);
    auto normal = flatEnd - flatStart;

    return std::any_of(segments.begin(), segments.end(), [flatStart, normal] (const Segment& segment) {
        return Intersects(flatStart, normal, segment.GetStartFlat(), segment.GetFlatNormal());
    });
}

bool geo::Intersects(glm::vec2 a, glm::vec2 adx, glm::vec2 b, glm::vec2 bdx) {
    float cross = adx.x * bdx.y - adx.y * bdx.x;

    if (cross == 0.0f) {
        return false;
    }

    glm::vec2 diff = b - a;
    float t = (diff.x * bdx.y - diff.y * bdx.x) / cross;

    // TODO: add propper epsilon
    return t > 0.001 && t < 0.999;
}

bool geo::IsClockwise(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    auto ab = b - a;
    auto ac = c - a;
    auto normal = glm::cross(ac, ab);
    return glm::dot(normal, glm::vec3(0,1.0f,0)) < 0;
}

float geo::Cross(glm::vec2 a, glm::vec2 b) {
    return a.x * b.y - a.y * b.x;
}

glm::vec2 geo::Flatten(glm::vec3 a) {
    return glm::vec2(a.x, a.z);
}

int geo::mod(int a, int b) {
    int remainder = a % b;
    if (remainder < 0)
        remainder += b < 0 ? -b : b;
    return remainder;
}

bool geo::IsInsideTriangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 point) {
    float d1 = Cross(b - a, point - a);
    float d2 = Cross(c - b, point - b);
    float d3 = Cross(a - c, point - c);
    return (d1 < 0 && d2 < 0 && d3 < 0) || (d1 > 0 && d2 > 0 && d3 > 0);
}
