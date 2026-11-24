#include "gizmos.hpp"
#include <glad/gl.h>
#include <random>

#include "camera.hpp"


std::shared_ptr<Gizmos> Gizmos::GetInstance() {
    if (_instance == nullptr) {
        _instance = std::make_shared<Gizmos>(Gizmos());
    }

    return _instance;
}

void Gizmos::DrawGizmos(std::shared_ptr<Camera> camera) {
    _shader->Bind();
    _shader->SetMatrix4x4("view", camera->GetMatrix());

    GLuint pointBuffer, lineBuffer;
    glGenBuffers(1, &pointBuffer);
    glGenBuffers(1, &lineBuffer);

    glPointSize(10.0f);
    glBindBuffer(GL_ARRAY_BUFFER, pointBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(_points.size() * 2 * sizeof(glm::vec3)), _points.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_POINTS, 0, _points.size());

    glLineWidth(2.0f);
    glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(_lines.size() * sizeof(Point)), _lines.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_LINES, 0, _lines.size());

    _points.clear();
    _lines.clear();
}

void Gizmos::DrawPoint(glm::vec3 point, glm::vec3 color) {
    _points.push_back({point, color});
}

void Gizmos::DrawLine(Segment segment, glm::vec3 color) {
    _lines.push_back({segment.Start, color});
    _lines.push_back({segment.End, color});
}

void Gizmos::DrawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color) {
    _lines.push_back({start, color});
    _lines.push_back({end, color});
}

void Gizmos::DrawVector(glm::vec3 start, glm::vec3 magnitude, glm::vec3 color) {
    DrawLine(start, start +  magnitude, color);
}

void Gizmos::DrawSegments(std::vector<Segment> segments, glm::vec3 color) {
    for (auto segment : segments) {
        DrawLine(segment, color);
    }
}

glm::vec3 Gizmos::Color(int hash) {
    float hue = glm::fract(hash * 0.618034f);
    float r = glm::abs(hue * 6.0f - 3.0f) - 1.0f;
    float g = 2.0f - glm::abs(hue * 6.0f - 2.0f);
    float b = 2.0f - glm::abs(hue * 6.0f - 4.0f);
    return glm::clamp(glm::vec3(r, g, b), 0.0f, 1.0f);
}

glm::vec3 Gizmos::Color(std::string hash) {
    return Color(std::hash<std::string>{}(hash) % 50);
}
