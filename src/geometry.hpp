#pragma once
#include "mesh.hpp"
#include "glm/glm.hpp"
#include "segment.hpp"
#include "vertex.hpp"

// Collection of function for manipulating geometry (basically blender operations)
namespace geo {
    /**
     * @return Closes point on line (a - b) from point
     */
    glm::vec3 ClosestPointOnLine(glm::vec3 a, glm::vec3 b, glm::vec3 point);

    /**
     * @return Distance to closes point on line (a - b) from point
     */
    float DistanceToLine(glm::vec3 a, glm::vec3 b, glm::vec3 point);

    /**
     * Point at which two line intersect
     */
    void Intersection(glm::vec2 a, glm::vec2 adx, glm::vec2 b, glm::vec2 bdx, bool &parallel, glm::vec2 &point);

    /**
    * Point at which two line intersect
    */
    void Intersection(Segment a, Segment b, bool &parallel, glm::vec2 &point);

    /**
     * Moves segments on y axis by amount
     */
    std::vector<Segment> Raise(std::vector<Segment> &segments, float amount);

    /**
     * Insets each segment by its right normal on y plane
     */
    std::vector<Segment> Inset(std::vector<Segment>& segments, float amount);

    /**
     * Connects neighbouring edges
     * @param segments vector of segments
     * @param loop if first and last segments should be connected
     * @return vector of connected segments
     */
    std::vector<Segment> ConnectNeighbours(std::vector<Segment>& segments, bool loop);

    /**
     * Creates mesh that bridges two segment paths
     */
    std::shared_ptr<Mesh> BridgeSegments(std::vector<Segment>& a, std::vector<Segment>& b);
}
