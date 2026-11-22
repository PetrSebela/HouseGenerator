#pragma once
#include <vector>

#include "wall.hpp"

class WallSet {
    std::vector<Wall> _walls;

public:
    WallSet(std::vector<std::vector<Segment>> &walls);
    WallSet(std::vector<Wall> &walls);
    std::shared_ptr<Mesh> GetMesh();
};
