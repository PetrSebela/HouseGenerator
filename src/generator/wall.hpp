#pragma once
#include <memory>

#include "mesh.hpp"
#include "segment.hpp"
#include "glm/vec3.hpp"

class Wall {
    glm::vec3 _bottomLeft, _bottomRight, _topLeft, _topRight;
public:
    Wall(Segment bottom, Segment top);
    std::shared_ptr<Mesh> GetMesh();
};
