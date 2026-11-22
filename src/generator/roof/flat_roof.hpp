#pragma once
#include "roof.hpp"


class FlatRoof : public Roof {

public:
    FlatRoof();
    FlatRoof(std::vector<Segment> perimeter);
    std::shared_ptr<Mesh> GetMesh() override;
};
