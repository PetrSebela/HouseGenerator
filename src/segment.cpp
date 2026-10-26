#include "segment.hpp"

#include "glm/glm.hpp"

float Segment::Length() {
    return glm::distance(Start, End);
}

glm::vec3 Segment::Normal() {
    return glm::cross(glm::vec3(0.0f,1.0f,0.0f), glm::normalize(End - Start));
}

Segment Segment::Offset(float amount) {
    glm::vec3 offset = Normal() * amount;
    return {Start + offset, End + offset};
}
