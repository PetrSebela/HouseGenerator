#include "footprint.hpp"

#include <iostream>
#include <ostream>

#include "editor.hpp"

void Footprint::UpdateLine() {
    _points.clear();
    for (auto handle : _handles)
        _points.push_back(handle.GetPosition());
    _outline.SetPoints(_points);
}

glm::vec3 Footprint::ClosestPointToLine(glm::vec3 a, glm::vec3 b, glm::vec3 cursor) {
    auto segmentLength = distance(a, b);
    auto cursorVector = cursor - a;
    auto lineVector = (b - a) / segmentLength;
    float distance = glm::dot(cursorVector, lineVector);
    if(distance <= 0)
        return a;
    if(distance >= segmentLength)
        return b;
    return a + lineVector * distance;
}

float Footprint::DistanceToLine(glm::vec3 a, glm::vec3 b, glm::vec3 cursor) {
    auto closest = ClosestPointToLine(a, b, cursor);
    return glm::distance(closest, cursor);
}

Footprint::Footprint(Editor* editor) {
    _editor = editor;

    Shader color("assets/shaders/vert.vert", "assets/shaders/color.frag");
    _outline = Line(color);
    _outline.SetLoop(true);

    _handles = {
        Handle(glm::vec3(0,0,1)),
        Handle(glm::vec3(1,0,1)),
        Handle(glm::vec3(0,0,0)),
    };

    // This callback is just pure mess, split it into more manageable chunks
    editor->RegisterEvent(SDL_EVENT_MOUSE_BUTTON_DOWN, [this, editor](const SDL_Event *event) {
        int closest = -1;
        auto cursor = editor->GetCursorWorldPosition();
        for (int i = 0; i < _handles.size(); i++) {
            if (_handles[i].Distance(cursor) < 0.2f) {
                closest = i;
            }
        }

        if (event->button.button == SDL_BUTTON_RIGHT && closest >= 0) {
            if (_handles.size() <= 3)
                return false;
            _handles.erase(_handles.begin() + closest);
            UpdateLine();
        }
        else if (event->button.button == SDL_BUTTON_LEFT && closest >= 0) {
            _draggingOutline = true;
            _selectedPointIndex = closest;
            return false;
        }

        if (event->button.button != SDL_BUTTON_LEFT)
            return false;

        for (int i = 0; i < _handles.size(); i++) {
            auto a = _handles[i].GetPosition();
            auto b = _handles[(i + 1) % _handles.size()].GetPosition();
            auto point = ClosestPointToLine(a, b, cursor);
            auto distance = glm::distance(point, cursor);

            if (distance < 0.2f && glm::distance(cursor, a) > 0.5f && glm::distance(cursor, b) > 0.5f ) {
                Handle handle(point);
                _handles.insert(_handles.begin() + i + 1, handle);
                _draggingOutline = true;
                _selectedPointIndex = i + 1;
                return false;
            }
        }

        return false;
    });

    editor->RegisterEvent(SDL_EVENT_MOUSE_BUTTON_UP, [this](const SDL_Event *event) {
        if (event->button.button == SDL_BUTTON_LEFT)
            _draggingOutline = false;
        return false;
    });

    editor->RegisterEvent(SDL_EVENT_MOUSE_MOTION, [this, editor](const SDL_Event *event) {
        if (!_draggingOutline)
            return false;

        auto cursor = editor->GetCursorWorldPosition();
        glm::vec3 target = cursor;

        if (Relax) {
            auto a = _handles[(_selectedPointIndex - 1) % _handles.size()].GetPosition();
            auto b = _handles[(_selectedPointIndex + 1) % _handles.size()].GetPosition();
            target = ClosestPointToLine(a, b, cursor);
        }
        else if (SnapToGrid) {
            target = glm::vec3(round(cursor.x / GridSize) * GridSize, 0, round(cursor.z / GridSize) * GridSize);
        }

        _handles[_selectedPointIndex].SetPosition(target);

        UpdateLine();
        return true;
    });

    editor->RegisterEvent(SDL_EVENT_KEY_DOWN, [this](const SDL_Event *event) {
        if (event->key.key == SDLK_S)
            SnapToGrid = !SnapToGrid;

        if (event->key.key == SDLK_R)
            Relax = !Relax;

        return false;
    });

    UpdateLine();
}

void Footprint::Draw(Camera& camera) {
    for (auto handle : _handles)
        handle.Draw(camera);
    _outline.Draw(camera);
}

std::vector<glm::vec3> Footprint::GetOutline() {
    return _points;
}
