#pragma once
#include "roof.hpp"


struct RoofPlaneDescriptor {
    float angle;
    float minRidgeHeight;
};

class GabledRoof : public Roof {

    std::vector<glm::vec3> FillRoof(glm::vec3 origin, glm::vec3 fillDirection, glm::vec3 rowDirection, std::vector<Segment>& segments);

public:
    glm::vec3 SplitPoint = glm::vec3(0.0f);
    bool MatchLeftRightRidge = true;
    RoofPlaneDescriptor LeftDescriptor{
        .angle = 15.f,
        .minRidgeHeight = 0.f,
    };
    RoofPlaneDescriptor RightDescriptor{
        .angle = 15.f,
        .minRidgeHeight = 0.f,
    };
    float RidgeAzimuth = 10.0f;

    GabledRoof(){}
    std::shared_ptr<Mesh> GetMesh() override;
};