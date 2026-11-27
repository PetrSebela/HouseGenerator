#pragma once
#include <memory>
#include <vector>
#include <string>
#include "mesh.hpp"
#include "scene_object.hpp"
#include "segment.hpp"

class Roof {
protected:
    std::shared_ptr<Mesh> _tile = Mesh::Load("assets/models/tile.obj");
    std::vector<Segment> _perimeter;

public:
    Roof();
    Roof(std::vector<Segment> perimeter);

    void SetPerimeter(std::vector<Segment> perimeter);

    /**
     * Based on https://alienryderflex.com/triangulation/
     */
    virtual std::vector<SceneObject> GetRoof();
};
