

#include "line.hpp"

#include <iostream>
#include <ostream>

#include "camera.hpp"

Line::Line(Shader shader) {
    _shader = shader;
}

Line::Line(std::vector<glm::vec3> points, Shader shader) : Line(shader) {
    SetPoints(points);
}

Line::~Line() {
}

void Line::SetLoop(bool loop) {
    _isLoop = loop;
}

void Line::Draw(Camera camera) {
    _shader.Bind();
    _shader.SetMatrix4x4("model", glm::identity<glm::mat4>());
    auto cameraMatrix = camera.GetViewMatrix();
    _shader.SetMatrix4x4("view", cameraMatrix);
    _mesh.Draw();
}

void Line::SetPoints(std::vector<glm::vec3> points) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<glm::vec3> segments = points;

    if (segments.size() < 2) {
        _mesh.Clear();
        return;
    }

    if (_isLoop && segments.size() > 2) {}
        segments.push_back(segments[0]);

    for (int i = 0; i < segments.size() - 1; i++) {
        auto start = segments[i];
        auto end = segments[i + 1];
        auto direction = glm::normalize(end - start);
        auto right = glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)) * 0.05f;

        Vertex vertexSR{
            .position = start + right
        };

        Vertex vertexSL{
            .position = start - right
        };

        Vertex vertexER{
            .position = end + right
        };

        Vertex vertexEL{
            .position = end - right
        };

        auto baseVertexIndex = vertices.size();

        vertices.push_back(vertexSR);
        vertices.push_back(vertexSL);
        vertices.push_back(vertexER);
        vertices.push_back(vertexEL);

        indices.push_back(baseVertexIndex);
        indices.push_back(baseVertexIndex + 1);
        indices.push_back(baseVertexIndex + 3);
        indices.push_back(baseVertexIndex);
        indices.push_back(baseVertexIndex + 2);
        indices.push_back(baseVertexIndex + 3);
    }

    _mesh.SetData(vertices, indices);
}
