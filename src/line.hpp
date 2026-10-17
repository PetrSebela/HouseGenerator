#include "camera.hpp"
#include "shader.hpp"
#include "mesh.hpp"

#ifndef _LINE_H_
#define _LINE_H_

class Line {
    Mesh _mesh;
    Shader _shader;
    bool _isLoop = false;
public:
    Line() = default;
    Line(Shader shader);
    Line(std::vector<glm::vec3> points, Shader shader);
    ~Line();
    void SetLoop(bool loop);
    void Draw(Camera camera);
    void SetPoints(std::vector<glm::vec3> points);
};


#endif
