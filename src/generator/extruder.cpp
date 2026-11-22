#include "extruder.hpp"

#include "geometry.hpp"

Extruder::Extruder(std::vector<Segment>& active) {
    _active = active;
    _frame.push_back(active);
    _loop = active.front().Start == active.back().End;
}

void Extruder::MoveHead(glm::vec2 offset) {
    _frame.pop_back();
    _active = geo::Raise(_active, offset.y);
    _active = geo::Inset(_active, offset.x);
    _active = geo::ConnectNeighbours(_active, _loop);
    _frame.push_back(_active);
}

void Extruder::Extrude(glm::vec2 offset) {
    _active = geo::Raise(_active, offset.y);
    _active = geo::Inset(_active, offset.x);
    _active = geo::ConnectNeighbours(_active, _loop);
    _frame.push_back(_active);
}

std::vector<Segment> Extruder::GetExtruderHead() {
    return _active;
}

std::vector<Wall> Extruder::GetWalls() {
    std::vector<Wall> walls;

    for (int frameIndex = 0; frameIndex < _frame.size() - 1; frameIndex++) {
        auto bottom = _frame[frameIndex];
        auto top = _frame[frameIndex + 1];

        for (int wallIndex = 0; wallIndex < _active.size(); wallIndex++) {
            Wall wall(bottom[wallIndex], top[wallIndex]);
            walls.push_back(wall);
        }
    }

    return walls;
}

WallSet Extruder::GetWallSet() {
    return WallSet(_frame);
}


