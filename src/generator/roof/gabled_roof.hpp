#pragma once
#include "geometry.hpp"
#include "roof.hpp"
#include "scene_object.hpp"


struct RoofPlaneDescriptor {
    float angle;
    float minRidgeHeight;
};

struct BisectResult {
    std::vector<Segment> left;
    std::vector<Segment> right;
    std::vector<Segment> combined;
};

class GabledRoof : public Roof {
    std::vector<glm::vec3> FillRoof(glm::vec3 origin, glm::vec3 fillDirection, glm::vec3 rowDirection, std::vector<Segment>& segments);
    BisectResult BisectPerimeter(std::vector<Segment>& perimeter, Segment& bisector);
    std::shared_ptr<Mesh> GetRoofBridgeWall(glm::vec3 splitDirection, Plane leftPlane, Plane rightPlane);
    std::vector<Segment> ProjectSegmentOnRoof(const std::vector<Segment>& segments, Plane left, Plane right, Segment ridge);
    std::vector<SceneObject> GetSimplifiedRoofMesh(Segment ridge, const std::vector<Segment>& segments, Plane leftPlane, Plane rightPlane, bool left);

public:
    glm::vec3 SplitPoint = glm::vec3(0.0f);
    bool MatchLeftRightRidge = true;
    bool HighQuality = false;

    RoofPlaneDescriptor LeftDescriptor{
        .angle = 15.f,
        .minRidgeHeight = 0.f,
    };
    RoofPlaneDescriptor RightDescriptor{
        .angle = 15.f,
        .minRidgeHeight = 0.f,
    };
    float RidgeAzimuth = 45.0f;

    GabledRoof(){}

    std::vector<SceneObject> GetRoof() override;
};