#include "segment.hpp"

#include "glm/glm.hpp"

float Segment::Length() const {
    return glm::distance(Start, End);
}

glm::vec3 Segment::Normal() const {
    return glm::cross(glm::vec3(0.0f,1.0f,0.0f), glm::normalize(End - Start));
}

glm::vec3 Segment::GetDirection() const{
    return glm::normalize(End - Start);
}

glm::vec2 Segment::GetStartFlat() const {
    return {Start.x, Start.z};
}

glm::vec2 Segment::GetEndFlat() const {
    return {End.x, End.z};
}

glm::vec2 Segment::GetFlatDirection() const {
    return glm::normalize(GetEndFlat() - GetStartFlat());
}

Segment Segment::Offset(float amount) const {
    glm::vec3 offset = Normal() * amount;
    return {Start + offset, End + offset};
}
