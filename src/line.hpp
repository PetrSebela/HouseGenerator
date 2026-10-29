#pragma once
#include "camera.hpp"
#include "shader.hpp"
#include "mesh.hpp"


class Line {
    std::shared_ptr<Mesh> _mesh = std::make_shared<Mesh>();
    Shader _shader = Shader("assets/shaders/default.vert","assets/shaders/color.frag");
    bool _isLoop = false;
public:
    Line() = default;
    Line(std::vector<glm::vec3> points);
    ~Line();
    void SetLoop(bool loop);
    void Draw(Camera camera);
    void SetPoints(std::vector<glm::vec3> points);
};