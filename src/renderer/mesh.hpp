#pragma once

#include <memory>

#include "vertex.hpp"

/**
 * Abstraction on OpenGL buffers representing mesh
 */
class Mesh {
    GLuint VAO, VBO, EBO;
    std::vector<Vertex> _vertices;
    std::vector<GLuint> _indices;

public:
    Mesh(const std::vector<Vertex>& vertices,const std::vector<GLuint>& indices);
    Mesh();
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void SetData(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);
    void Draw();
    void Clear();
    void Join(std::shared_ptr<Mesh> joined);

    static std::shared_ptr<Mesh> Stamp(std::shared_ptr<Mesh>& stamp, std::vector<glm::vec3>& positions);
    static std::shared_ptr<Mesh> Join(std::shared_ptr<Mesh> a, std::shared_ptr<Mesh> b);
    static std::shared_ptr<Mesh> Cube();
    static std::shared_ptr<Mesh> Plane(float size);
    static std::shared_ptr<Mesh> Quad();
    static std::shared_ptr<Mesh> Load(std::string path);
    static std::shared_ptr<Mesh> Transform(std::shared_ptr<Mesh> mesh, glm::mat4 transform);
};