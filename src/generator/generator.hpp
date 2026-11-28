#pragma once
#include <vector>

#include "roof/roof.hpp"
#include "renderer/camera.hpp"
#include "renderer/mesh.hpp"
#include "renderer/scene_object.hpp"
#include "renderer/shader.hpp"
#include "glm/glm.hpp"
#include "segment.hpp"
#include "roof/gabled_roof.hpp"

/**
 * The house generator himself
 */
class Generator {
    std::vector<Segment> _footprint; // Base building footprint segments
    std::shared_ptr<SceneObject> _foundation = std::make_shared<SceneObject>();
    std::shared_ptr<SceneObject> _wall = std::make_shared<SceneObject>();
    // std::shared_ptr<SceneObject> _roof = std::make_shared<SceneObject>();

    std::vector<SceneObject> _roof;
public:
    std::shared_ptr<Roof> RoofGenerator = std::make_shared<Roof>();

    // Building parameters
    float Levels = 1;           // Number of stories
    float LevelHeight = 2.4f;   // Story height in meters

    float FoundationHeight = 0.4f;

    
    Generator();
    ~Generator() = default;

    /**
     * Updates building footprint
     * @param footprint Building footprint
     */
    void SetFootprint(std::vector<glm::vec3> footprint);

    /**
     * Regenerates building model
     */
    void RefreshModel();

    /**
     * Draws building model
     */
    void Draw();
};
