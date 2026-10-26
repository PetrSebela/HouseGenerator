#include "generator.hpp"

#include "camera.hpp"
#include "geometry.hpp"

void Generator::SetFootprint(std::vector<glm::vec3> footprint) {
    _footprint.clear();

    for (int i = 0; i < footprint.size(); i++) {
        Segment segment(footprint[i], footprint[(i + 1) % footprint.size()]);
        _footprint.push_back(segment);
    }

    RefreshModel();
}

void Generator::RefreshModel() {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    // std::vector<glm::vec3> outline;
    //
    // for (int i = 0; i < _footprint.size(); i++) {
    //     auto a = _footprint[i];
    //     auto b = _footprint[(i + 1) % _footprint.size()];
    //     auto normal = glm::cross(glm::vec3(0,1,0), glm::normalize(b - a));
    //
    //     outline.push_back(a + normal * 0.25f);
    //     outline.push_back(b + normal * 0.25f);
    // }
    //
    // std::vector<glm::vec3> outlineCropped;
    //
    // for (int i = 0; i < outline.size(); i+=2) {
    //     auto a = outline[i];
    //     auto b = outline[(i + 1) % outline.size()];
    //     auto c = outline[(i + 2) % outline.size()];
    //     auto d = outline[(i + 3) % outline.size()];
    //
    //     glm::vec2 af = glm::vec2(a.x,a.z);
    //     glm::vec2 bf = glm::vec2(b.x,b.z);
    //     glm::vec2 cf = glm::vec2(c.x,c.z);
    //     glm::vec2 df = glm::vec2(d.x,d.z);
    //
    //     glm::vec2 adx = bf - af;
    //     glm::vec2 bdx = df - cf;
    //     bool parallel;
    //     glm::vec2 intersection;
    //     geo::Intersection(af, adx, cf, bdx, parallel, intersection);
    //
    //     glm::vec3 intersected = glm::vec3(intersection.x, 0, intersection.y);
    //     outlineCropped.push_back(intersected);
    // }

    for ( auto segment : _footprint) {
        auto normal = segment.Normal();
        auto base = vertices.size();

        vertices.push_back({.position = segment.Start, .normal = normal, .UV = glm::vec2(0,0)});
        vertices.push_back({.position = segment.End,.normal = normal, .UV = glm::vec2(segment.Length(),0)});
        vertices.push_back({.position = segment.Start + glm::vec3(0.0f, LevelHeight, 0.0f),.normal = normal, .UV = glm::vec2(0,LevelHeight)});
        vertices.push_back({.position = segment.End + glm::vec3(0.0f, LevelHeight, 0.0f),.normal = normal, .UV = glm::vec2(segment.Length(),LevelHeight)});

        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 2);
        indices.push_back(base + 1);
        indices.push_back(base + 3);
    }

    _walls.SetData(vertices, indices);
}

void Generator::Draw(Camera camera) {
    _walls.Draw();
}