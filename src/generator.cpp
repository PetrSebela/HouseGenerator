#include "generator.hpp"

#include "camera.hpp"

void Generator::SetFootprint(std::vector<glm::vec3> footprint) {
    _footprint = footprint;
    RefreshModel();
}

void Generator::RefreshModel() {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    for (int i = 0; i < _footprint.size(); i++) {
        auto a = _footprint[i];
        auto b = _footprint[(i + 1) % _footprint.size()];
        auto width = glm::distance(a, b);
        auto normal = glm::cross(glm::vec3(0,1,0), glm::normalize(b - a));

        auto base = vertices.size();
        vertices.push_back({.position = a,.normal = normal, .UV = glm::vec2(0,0)});
        vertices.push_back({.position = b,.normal = normal, .UV = glm::vec2(width,0)});
        vertices.push_back({.position = a + glm::vec3(0.0f, LevelHeight, 0.0f),.normal = normal, .UV = glm::vec2(0,LevelHeight)});
        vertices.push_back({.position = b + glm::vec3(0.0f, LevelHeight, 0.0f),.normal = normal, .UV = glm::vec2(width,LevelHeight)});

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
    // _shader.Bind();
    // _shader.SetMatrix4x4("model", glm::identity<glm::mat4>());
    // auto cameraMatrix = camera.GetViewMatrix();
    // _shader.SetMatrix4x4("view", cameraMatrix);
    _walls.Draw();
}