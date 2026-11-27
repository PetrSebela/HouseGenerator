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

glm::vec2 geo::Intersection(glm::vec2 a, glm::vec2 adx, glm::vec2 b, glm::vec2 bdx, bool &parallel, glm::vec2 &point) {
    float cross = adx.x * bdx.y - adx.y * bdx.x;
    point = glm::vec2(0);

    if (std::abs(cross) <= 1e-3f * glm::length(adx) * glm::length(bdx)) {
        parallel = true;
        return glm::vec2(std::numeric_limits<float>::quiet_NaN());
    }

    glm::vec2 diff = b - a;
    float t = (diff.x * bdx.y - diff.y * bdx.x) / cross;
    float u = (diff.x * adx.y - diff.y * adx.x) / cross;

    point = a + adx * t;
    parallel = false;
    return {t,u};
}

glm::vec2 geo::Intersection(Segment a, Segment b, bool &parallel, glm::vec2 &point) {
    return Intersection(a.GetStartFlat(),
                    a.GetFlatDirection(),
                    b.GetStartFlat(),
                    b.GetFlatDirection(),
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
        if (i != 0 || loop) {
            Intersection(current, before, parallel, start);
            if (parallel)
                start = current.GetStartFlat();
        }

        glm::vec2 end = current.GetEndFlat();
        if (i != segments.size() - 1 || loop) {
            Intersection(current, after, parallel, end);
            if (parallel)
                end = current.GetEndFlat();
        }

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

bool geo::GetSegmentSide(const Segment& segment, const  Segment& bisector) {
    auto relativeStart = geo::Flatten(segment.Start - bisector.Start);
    auto relativeEnd = geo::Flatten(segment.End - bisector.End);
    auto startCross = geo::Cross(relativeStart, geo::Flatten(bisector.GetDirection()));
    auto endCross = geo::Cross(relativeEnd, geo::Flatten(bisector.GetDirection()));
    return startCross + endCross > 0;
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
        return Intersects(flatStart, normal, segment.GetStartFlat(), segment.GetFlatDirection());
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

float geo::Distance(glm::vec3 a, Plane plane) {
    return glm::dot(a - plane.point, glm::normalize(plane.normal));
}

glm::vec3 geo::Intersects(glm::vec3 point, glm::vec3 direction, Plane plane) {
    const float denom = glm::dot(plane.normal, direction);

    if (glm::abs(denom) < 1e-8f)
        return glm::vec3(std::numeric_limits<float>::quiet_NaN());

    const float t = glm::dot(plane.point - point, plane.normal) / denom;

    if (t < 0.0f)
        return glm::vec3(std::numeric_limits<float>::quiet_NaN());

    return point + t * direction;
}

std::vector<float> geo::GetIntersections(glm::vec3 point, glm::vec3 direction, std::vector<Segment>& segments) {
    auto flatPoint = Flatten(point);
    auto flatDirection = Flatten(direction);
    std::vector<float> intersections;

    for (const auto& segment : segments) {
        bool parallel;
        glm::vec2 point;
        auto distances = Intersection(flatPoint, flatDirection, segment.GetStartFlat(), segment.GetFlatDirection(), parallel, point);

        if (parallel || distances.y < 0 || distances.y > segment.Length()) {
            continue;
        }

        intersections.push_back(distances.x);
    }

    std::sort(intersections.begin(), intersections.end());
    return intersections;
}

float geo::Angle(glm::vec3 a, glm::vec3 b) {
    return glm::acos(glm::dot(glm::normalize(a), glm::normalize(b))) * 180.0f / glm::pi<float>();
}

bool geo::IsInsideTriangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 point) {
    float d1 = Cross(b - a, point - a);
    float d2 = Cross(c - b, point - b);
    float d3 = Cross(a - c, point - c);
    return (d1 < 0 && d2 < 0 && d3 < 0) || (d1 > 0 && d2 > 0 && d3 > 0);
}
