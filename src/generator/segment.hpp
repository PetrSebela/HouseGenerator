#pragma once
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

/**
 * Represents building wall segment
 */
class Segment {
public:
    glm::vec3 Start;    // Segment start
    glm::vec3 End;      // Segment end

    Segment(glm::vec3 start, glm::vec3 end) : Start(start), End(end) {};
    ~Segment() = default;

    /**
     * @return Segment length
     */
    float Length();

    /**
     * @return Segment normal on y-plane
     */
    glm::vec3 Normal();

    /**
     * @return Segment direction vector
     */
    glm::vec3 Direction();

    /**
     * @return Segment start on y-plane
     */
    glm::vec2 GetStartFlat();

    /**
     * @return Segment end on y-plane
     */
    glm::vec2 GetEndFlat();

    /**
     * @return Segment direction on y-plane
     */
    glm::vec2 GetDirectionFlat();

    /**
     * Offsets segment along its normal
     * @param amount by which the segment is offseted
     * @return offseted segment
     */
    Segment Offset(float amount);
};
