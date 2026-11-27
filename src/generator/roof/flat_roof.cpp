#include "flat_roof.hpp"
#include "flat_roof.hpp"
#include "../geometry.hpp"

FlatRoof::FlatRoof() {
}

FlatRoof::FlatRoof(std::vector<Segment> perimeter) : Roof(perimeter) {}

std::vector<SceneObject> FlatRoof::GetRoof() {
    return{
        SceneObject(geo::Fill(_perimeter), Shader::GetShaderByName("wall"))
    };
}
