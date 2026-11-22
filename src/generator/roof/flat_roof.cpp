#include "flat_roof.hpp"
#include "flat_roof.hpp"
#include "../geometry.hpp"

FlatRoof::FlatRoof() {
}

FlatRoof::FlatRoof(std::vector<Segment> perimeter) : Roof(perimeter) {}

std::shared_ptr<Mesh> FlatRoof::GetMesh() {
    return geo::Fill(_perimeter);
}
