#pragma once
#include <memory>
#include <vector>

#include "mesh.hpp"
#include "segment.hpp"

class Roof {
protected:
    std::vector<Segment> _perimeter;

public:
    Roof();
    Roof(std::vector<Segment> perimeter);

    void SetPerimeter(std::vector<Segment> perimeter);

    /**
     * Based on https://alienryderflex.com/triangulation/
     */
    virtual std::shared_ptr<Mesh> GetMesh();
};
