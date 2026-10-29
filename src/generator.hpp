#pragma once
#include <vector>

#include "camera.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "glm/glm.hpp"
#include "segment.hpp"

class Generator {
    std::vector<Segment> _footprint;
    Shader _shader = Shader("assets/shaders/default.vert", "assets/shaders/wall.frag");
    std::shared_ptr<Mesh> _walls;

public:
    float Levels = 1;
    float LevelHeight = 2.4f;

    Generator() = default;
    ~Generator() = default;
    void SetFootprint(std::vector<glm::vec3> footprint);
    void RefreshModel();
    void Draw(Camera camera);
};
