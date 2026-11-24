#pragma once
#include "roof.hpp"


class GabledRoof : public Roof {
    float roofAngle;

    std::vector<glm::vec3> FillRoof(glm::vec3 origin, glm::vec3 fillDirection, glm::vec3 rowDirection, std::vector<Segment>& segments);

public:

    GabledRoof(){}
    std::shared_ptr<Mesh> GetMesh() override;
};