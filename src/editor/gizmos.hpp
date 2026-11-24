#pragma once
#include <memory>
#include <memory>
#include <vector>

#include "camera.hpp"
#include "mesh.hpp"
#include "segment.hpp"
#include "shader.hpp"
#include "glm/vec3.hpp"

/**
 * Struct representing art of a gizmo
 */
struct Point {
    glm::vec3 point;
    glm::vec3 color;
};

/**
 * Class responsible for drawing gizmos
 */
class Gizmos {
    std::shared_ptr<Shader> _shader = Shader::LoadShader("assets/shaders/gizmo.vert","assets/shaders/gizmo.frag");  // Shader which is used for drawing gizmos
    std::vector<Point> _points;     // Collection of points to be drawn
    std::vector<Point> _lines;      // Collection of line points to be drawn

    inline static std::shared_ptr<Gizmos> _instance;    // Singleton instance
    Gizmos(){}

public:
    /**
     * @return Singleton instance
     */
    static std::shared_ptr<Gizmos> GetInstance();

    /**
     * Draws all gizmos that were requested this frame
     * @param camera camera whose perspective is be used for rendering
     */
    void DrawGizmos(std::shared_ptr<Camera> camera);

    /**
     * Requests drawing of point gizmo in this frame
     */
    void DrawPoint(glm::vec3 point, glm::vec3 color = glm::vec3(1.0,0.0,1.0));

    /**
     * Requests drawing of line gizmo in this frame
     */
    void DrawLine(Segment segment, glm::vec3 color = glm::vec3(1.0,0.0,1.0));

    /**
     * Requests drawing of line gizmo in this frame
     */
    void DrawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color = glm::vec3(1.0,0.0,1.0));
    /**
     * Requests drawing of vector gizmo in this frame
     */
    void DrawVector(glm::vec3 start, glm::vec3 magnitude, glm::vec3 color = glm::vec3(1.0,0.0,1.0));

    /**
     * Draws vector of segments
     */
    void DrawSegments(std::vector<Segment> segments, glm::vec3 color = glm::vec3(1.0,0.0,1.0));

    static glm::vec3 Color(int hash);

    static glm::vec3 Color(std::string hash);

    inline static glm::vec3 RED = glm::vec3(1,0,0);
    inline static glm::vec3 GREEN = glm::vec3(0,1,0);
    inline static glm::vec3 BLUE = glm::vec3(0,0,1);
    inline static glm::vec3 YELLOW = glm::vec3(1,1,0);
    inline static glm::vec3 CYAN = glm::vec3(0,1,1);
    inline static glm::vec3 MAGENTA = glm::vec3(1,0,1);
    inline static glm::vec3 BLACK = glm::vec3(0,0,0);
    inline static glm::vec3 GRAY = glm::vec3(0.25,0.25,0.25);

};
