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
    float Length() const;

    /**
     * @return Segment normal on y-plane
     */
    glm::vec3 Normal() const;

    /**
     * @return Segment direction vector
     */
    glm::vec3 GetDirection() const;

    /**
     * @return Segment start on y-plane
     */
    glm::vec2 GetStartFlat() const;

    /**
     * @return Segment end on y-plane
     */
    glm::vec2 GetEndFlat() const;

    /**
     * @return Segment direction on y-plane
     */
    glm::vec2 GetFlatNormal() const;

    /**
     * Offsets segment along its normal
     * @param amount by which the segment is offseted
     * @return offseted segment
     */
    Segment Offset(float amount) const;
};
