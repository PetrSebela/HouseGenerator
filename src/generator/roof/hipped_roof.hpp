#pragma once
#include "flat_roof.hpp"
#include "roof.hpp"


class HippedRoof : public Roof {

public:
    HippedRoof();
    std::shared_ptr<Mesh> GetMesh() override;
};