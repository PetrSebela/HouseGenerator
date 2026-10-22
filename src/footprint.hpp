#pragma once
#include <vector>
#include "handle.hpp"
#include "line.hpp"
#include "glm/vec3.hpp"

class Editor;

class Footprint {
    std::vector<Handle> _handles;
    std::vector<glm::vec3> _points;
    Line _outline;
    Editor* _editor;
    bool _draggingOutline = false;
    int _selectedPointIndex = 0;


    void UpdateLine();

public:
    bool Relax = false;
    bool SnapToGrid = false;
    float GridSize = 0.5f;

    Footprint() = default;
    Footprint(Editor* editor);

    // Delete copy constructors because of lambda functions use 'this' keyword in capture group
    Footprint(const Footprint&) = delete;
    Footprint& operator=(const Footprint&) = delete;

    void Draw(Camera& camera);
    std::vector<glm::vec3> GetOutline();
};