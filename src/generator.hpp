#pragma once
#include <vector>

#include "camera.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "glm/glm.hpp"
#include "segment.hpp"

/**
 * The house generator himself
 */
class Generator {
    std::vector<Segment> _footprint; // Base building footprint segments

    // TMP variables for debug before deciding on model structure
    Shader _shader = Shader("assets/shaders/default.vert", "assets/shaders/wall.frag");
    std::shared_ptr<Mesh> _walls;

public:
    // Building parameters
    float Levels = 1;           // Number of stories
    float LevelHeight = 2.4f;   // Story height in meters

    Generator() = default;
    ~Generator() = default;

    /**
     * Updates building footprint (for now also triggers regeneration)
     * @param footprint Building footprint
     */
    void SetFootprint(std::vector<glm::vec3> footprint);

    /**
     * Regenerates building model
     */
    void RefreshModel();

    /**
     * Draws building model
     * @param camera Camera used for rendering
     */
    void Draw(Camera camera);
};
