#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices) : Mesh() {

    SetData(vertices, indices);
}

Mesh::Mesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void Mesh::SetData(std::vector<Vertex> vertices, std::vector<GLuint> indices) {
    // TODO: add validity checks
    _vertices = vertices;
    _indices = indices;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Mesh::Draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::Clear() {
    // Lets give the data jump rope and let it jump all over the memory :3
    _vertices.clear();
    _indices.clear();
    SetData(_vertices, _indices);
}

Mesh Mesh::Box() {
    Vertex vertices[] = {
        Vertex{glm::vec3(-1, 0, -1), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
        Vertex{glm::vec3(1, 0, -1), glm::vec3(0, 0, 0), glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 2, -1), glm::vec3(0, 0, 0), glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 2, -1), glm::vec3(0, 0, 0), glm::vec2(1, 1)},

        Vertex{glm::vec3(-1, 0, 1), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
        Vertex{glm::vec3(1, 0, 1), glm::vec3(0, 0, 0), glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 2, 1), glm::vec3(0, 0, 0), glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 2, 1), glm::vec3(0, 0, 0), glm::vec2(1, 1)},

        Vertex{glm::vec3(1, 0, -1), glm::vec3(1, 1, 1), glm::vec2(0, 0)},
        Vertex{glm::vec3(1, 0, 1), glm::vec3(0, 1, 0), glm::vec2(1, 0)},
        Vertex{glm::vec3(1, 2, -1), glm::vec3(0, 0, 1), glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 2, 1), glm::vec3(1, 0, 0), glm::vec2(1, 1)},

        Vertex{glm::vec3(-1, 0, 1), glm::vec3(1, 1, 1), glm::vec2(0, 0)},
        Vertex{glm::vec3(-1, 0, -1), glm::vec3(1, 0, 0), glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 2, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1)},
        Vertex{glm::vec3(-1, 2, -1), glm::vec3(0, 0, 1), glm::vec2(1, 1)},
    };
    GLuint indices[] = {
        0, 1, 3,
        0, 2, 3,
        4, 5, 7,
        4, 6, 7,
        8, 9, 11,
        8, 10, 11,
        12, 13, 15,
        12, 14, 15,
    };
    auto vert = std::vector(vertices, vertices + 16);
    auto ind = std::vector(indices, indices + 24);
    return Mesh(vert, ind);
}

Mesh Mesh::Plane(float size) {
    std::vector<Vertex> vertices = {
        Vertex{glm::vec3(-1, 0, -1) * size, glm::vec3(0, 0, 0), glm::vec2(0, 0)},
        Vertex{glm::vec3(1, 0, -1) * size, glm::vec3(0, 0, 0), glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 0, 1) * size, glm::vec3(0, 0, 0), glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 0, 1) * size, glm::vec3(0, 0, 0), glm::vec2(1, 1)}
    };

    std::vector<GLuint> indices = {
        0, 1, 3,
        0, 2, 3
    };

    return Mesh(vertices, indices);
}
