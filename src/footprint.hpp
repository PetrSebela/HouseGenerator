#pragma once
#include <vector>
#include "handle.hpp"
#include "line.hpp"
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

    /**
     * Update line visualization
     */
    void UpdateLine();

public:
    // Control flags
    bool Relax = false;         // If dragged handle should be placed on line between adjacent points
    bool SnapToGrid = false;    // If dragged handle should be snapped to grid
    float GridSize = 0.5f;      // Snap grid size

    Footprint() = default;
    Footprint(Editor* editor);

    // Delete copy and move constructors because of event callback
    // use 'this' keyword in capture group and it should always reference the same object
    Footprint(const Footprint&) = delete;
    Footprint& operator=(const Footprint&) = delete;

    /**
     * Draws handles and outline
     * @param camera Camera which should be used for drawing said objects
     */
    void Draw(Camera& camera);

    /**
     * @return Returns position of control points
     */
    std::vector<glm::vec3> GetOutline();
};