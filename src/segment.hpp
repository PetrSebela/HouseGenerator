#pragma once
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

class Segment {

public:
    glm::vec3 Start;
    glm::vec3 End;
    Segment(glm::vec3 start, glm::vec3 end) : Start(start), End(end) {};
    ~Segment() = default;
    float Length();
    glm::vec3 Normal();
    glm::vec3 Direction();

    glm::vec2 GetStartFlat();
    glm::vec2 GetEndFlat();
    glm::vec2 GetDirectionFlat();

    Segment Offset(float amount);
};
