#include "roof.hpp"

#include "geometry.hpp"
#include "scene_object.hpp"

Roof::Roof() {
}

Roof::Roof(std::vector<Segment> perimeter) {
    _perimeter = perimeter;
}

void Roof::SetPerimeter(std::vector<Segment> perimeter) {
    _perimeter = perimeter;
}

std::vector<SceneObject> Roof::GetRoof() {
    return {};
}
