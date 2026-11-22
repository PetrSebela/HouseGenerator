#pragma once

#include "segment.hpp"
#include "wall.hpp"
#include "wall_set.hpp"

class Extruder {
    std::vector<Segment> _active;
    std::vector<std::vector<Segment>> _frame;
    bool _loop;

public:
    Extruder(std::vector<Segment> &active);

    void MoveHead(glm::vec2 offset);
    void Extrude(glm::vec2 offset);
    std::vector<Segment> GetExtruderHead();
    std::vector<Wall> GetWalls();
    WallSet GetWallSet();
};
