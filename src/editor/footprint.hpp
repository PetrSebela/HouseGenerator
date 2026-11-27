#pragma once
#include <vector>
#include "handle.hpp"
#include "renderer/line.hpp"
#include "glm/vec3.hpp"

// To resolve circular dependency
class Editor;

/**
 * Editor of building footprints
 */
class Footprint {
    std::vector<Handle> _handles;   // Footprint control points
    std::vector<glm::vec3> _points; // Locations of control points in world space
    Line _outline;                  // Outline visualization
    Editor* _editor;                // Reference to editor (for getting cursor position)
    bool _draggingOutline = false;  // Flag representing if user drags outline
    int _selectedPointIndex = 0;    // Which point the user is dragging
    bool _isModified = true;

    // Following shapes cannot be static, as they create their own opengl object which is not initialized at that point
    std::vector<Handle> L_SHAPE = {
        Handle(glm::vec3(5,0,3)),
        Handle(glm::vec3(5,0,0)),
        Handle(glm::vec3(7,0,0)),
        Handle(glm::vec3(7,0,-3)),
        Handle(glm::vec3(-5,0,-3)),
        Handle(glm::vec3(-5,0,3)),
    };

    std::vector<Handle> BOX_SHAPE = {
        Handle(glm::vec3(5,0,5)),
        Handle(glm::vec3(5,0,-5)),
        Handle(glm::vec3(-5,0,-5)),
        Handle(glm::vec3(-5,0,5)),
    };

    /**
     * Update line visualization
     */
    void UpdateLine();

public:
    // Control flags
    bool Relax = false;         // If dragged handle should be placed on line between adjacent points
    bool SnapToGrid = false;    // If dragged handle should be snapped to grid
    float GridSize = 0.5f;      // Snap grid size

    /**
     * Check is footprint was modified since last check
     * @warning Reading resets value back to false until another modification event
     */
    bool WasModified();

    Footprint() = default;
    Footprint(Editor* editor);

    // Delete copy and move constructors because of event callback
    // use 'this' keyword in capture group and it should always reference the same object
    Footprint(const Footprint&) = delete;
    Footprint& operator=(const Footprint&) = delete;

    /**
     * Draws handles and outline
     */
    void Draw();

    /**
     * @return Returns position of control points
     */
    std::vector<glm::vec3> GetOutline();
};