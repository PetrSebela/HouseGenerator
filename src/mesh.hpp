#pragma once

#include <memory>

#include "vertex.hpp"

class Mesh {
    GLuint VAO, VBO, EBO;
    std::vector<Vertex> _vertices;
    std::vector<GLuint> _indices;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
    Mesh();
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void SetData(std::vector<Vertex> vertices, std::vector<GLuint> indices);
    void Draw();
    void Free();
    void Clear();

    static std::shared_ptr<Mesh> Join(std::shared_ptr<Mesh> a, std::shared_ptr<Mesh> b);
    static std::shared_ptr<Mesh> Box();
    static std::shared_ptr<Mesh> Plane(float size);
    static std::shared_ptr<Mesh> Quad();
};