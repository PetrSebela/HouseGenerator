#include "wall.hpp"

Wall::Wall(Segment bottom, Segment top) {
    _bottomLeft = bottom.Start;
    _bottomRight = bottom.End;
    _topLeft = top.Start;
    _topRight = top.End;
}

std::shared_ptr<Mesh> Wall::GetMesh() {
    std::vector<GLuint> indices {0,1,2,1,2,3};

    auto rightDirection = glm::normalize(_bottomRight - _bottomLeft);
    glm::vec3 normal = glm::cross(rightDirection, glm::normalize(_topLeft - _bottomLeft));

    float height = glm::distance(_bottomLeft, _topLeft);
    float width = glm::distance(_bottomLeft, _bottomRight);

    std::vector<Vertex> vertices {
    {
            .position = _bottomLeft,
            .normal = normal,
            .tangent = rightDirection,
            .UV = glm::vec2(0, 0),
        },
    {
            .position = _bottomRight,
            .normal = normal,
            .tangent = rightDirection,
            .UV = glm::vec2(width, 0),
        },
    {
            .position = _topLeft,
            .normal = normal,
            .tangent = rightDirection,
            .UV = glm::vec2(0, height),
        },
    {
            .position = _topRight,
            .normal = normal,
            .tangent = rightDirection,
            .UV = glm::vec2(width, height),
        },
    };

    return std::make_shared<Mesh>(vertices, indices);
}
