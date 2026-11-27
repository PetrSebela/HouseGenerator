#pragma once
#include "flat_roof.hpp"
#include "roof.hpp"


class HippedRoof : public Roof {

public:
    HippedRoof();
    std::vector<SceneObject> GetRoof() override;
};