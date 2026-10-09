#ifndef _MESH_H_
#define _MESH_H_

#include "vertex.hpp"

class Mesh {
    GLuint VAO, VBO, EBO;
    std::vector<Vertex> _vertices;
    std::vector<GLuint> _indices;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
    Mesh();
    ~Mesh();
    void SetData(std::vector<Vertex> vertices, std::vector<GLuint> indices);
    void Draw();
};

#endif
