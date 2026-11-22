#include "roof.hpp"

#include "geometry.hpp"

Roof::Roof() {
}

Roof::Roof(std::vector<Segment> perimeter) {
    _perimeter = perimeter;
}

void Roof::SetPerimeter(std::vector<Segment> perimeter) {
    _perimeter = perimeter;
}

std::shared_ptr<Mesh> Roof::GetMesh() {
    return std::make_shared<Mesh>();
}
