#pragma once
#include "roof.hpp"


class FlatRoof : public Roof {

public:
    FlatRoof();
    FlatRoof(std::vector<Segment> perimeter);
    std::vector<SceneObject> GetRoof() override;
};
