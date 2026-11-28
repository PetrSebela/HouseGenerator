#pragma once
#include "geometry.hpp"
#include "roof.hpp"
#include "scene_object.hpp"

struct RoofPlaneDescriptor {
    float angle;            // Roof angle from horizontal plane
    float minRidgeHeight;   // Minimum height of roof ridge
};

/**
 * Class representing generation of gabled roofs
 * https://wiki.openstreetmap.org/wiki/Tag:roof:shape%3Dgabled
 */
class GabledRoof : public Roof {
    Plane leftPlane;    // Left roof plane
    Plane rightPlane;   // Right roof plane

    glm::vec3 leftNormal;   // Left plane normal
    glm::vec3 rightNormal;  // Right plane normal
    glm::vec3 leftTangent;  // Left plane tangent (used as fill direction for tiles)
    glm::vec3 rightTangent; // Right plane tangent (used as fill direction for tiles)

    Segment ridge;  // Desired ridge splitting the two roof sides

    /**
     * Fills provided regions using line fill algorithm
     * @param origin from what point should the fill start
     * @param columnDirection in what direction should whe columns be filled
     * @param rowDirection in what direction should the rows be filled
     * @param segments segments defining filled region
     * @return vector of points at which will be
     */
    std::vector<glm::vec3> FillRegions(glm::vec3 origin, glm::vec3 columnDirection, glm::vec3 rowDirection, std::vector<Segment>& segments);

    /**
     * @return Mesh that fills vertical void between two roof sides if they are at different heights
     */
    std::shared_ptr<Mesh> GetRoofBridgeWall();

    /**
     * Projects segments onto the roof
     */
    std::vector<Segment> ProjectOnRoof(const std::vector<Segment> &segments);

    /**
     * Generates rain gutters
     */
    std::vector<SceneObject> GenerateGutters(std::vector<std::vector<Segment>> segments);

public:
    glm::vec3 SplitPoint = glm::vec3(0.0f);   // Point through which the roof ridge passes (y coordinate is ignored)
    bool MatchLeftRightRidge = true;                // If left and right side ridges should match
    bool HighQuality = false;                       // If high quality roof model should be generated

    RoofPlaneDescriptor LeftDescriptor;     // Left roof descriptor
    RoofPlaneDescriptor RightDescriptor;    // Right roof descriptor
    float RidgeAzimuth = 45.0f;             // Roof ridge direction

    GabledRoof();

    /**
     * Updates generator values based on provided parameter
     */
    void UpdateConfig();

    /**
     * Generates roof objects
     * @return vector of scene objects defining roof
     */
    std::vector<SceneObject> GetRoof() override;
};