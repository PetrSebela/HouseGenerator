#include "mesh.hpp"

#include <fstream>
#include <iostream>
#include <ostream>

#include "utils.hpp"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) : Mesh() {
    SetData(vertices, indices);
}

Mesh::Mesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::SetData(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) {
    if (vertices.empty() || indices.empty()) {
        return;
    }

    _vertices = vertices;
    _indices = indices;


    GLint maxAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
    std::cout << "max: " << maxAttribs << std::endl;
    std::cout << "size: " << sizeof(Vertex) << std::endl;
    std::cout << "size: " << sizeof(glm::vec3) << std::endl;
    std::cout << "size: " << sizeof(glm::vec2) << std::endl;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(9* sizeof(float)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(12* sizeof(float)));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
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

void Mesh::Join(std::shared_ptr<Mesh> joined) {
    int offset = _vertices.size();
    _indices.reserve(_indices.size() + joined->_indices.size());
    for (auto index: joined->_indices) {
        _indices.push_back(offset + index);
    }

    _vertices.reserve(_vertices.size() + joined->_vertices.size());
    _vertices.insert(_vertices.end(), joined->_vertices.begin(), joined->_vertices.end());
    SetData(_vertices, _indices);
}

std::shared_ptr<Mesh> Mesh::Stamp(std::shared_ptr<Mesh> &stamp, std::vector<glm::vec3> &positions) {
    const auto& stampVertices = stamp->_vertices;
    const auto& stampIndices  = stamp->_indices;
    const auto vertexCount  = stampVertices.size();

    std::vector<Vertex> vertices(stampVertices.size() * positions.size());
    std::vector<GLuint> indices(stampIndices.size() * positions.size());

    for (auto copy = 0; copy < positions.size(); copy++) {
        for (auto vertexIndex = 0; vertexIndex < stampVertices.size(); vertexIndex++) {
            auto vertex = stampVertices[vertexIndex];
            vertex.position += positions[copy];
            vertices[copy * stampVertices.size() + vertexIndex] = vertex;
        }

        for (auto i = 0; i < stampIndices.size(); i++) {
            indices[copy * stampIndices.size() + i] = stampIndices[i] + copy * vertexCount;
        }
    }

    return std::make_shared<Mesh>(std::move(vertices), std::move(indices));
}

std::shared_ptr<Mesh> Mesh::Join(std::shared_ptr<Mesh> a, std::shared_ptr<Mesh> b) {
    std::vector<Vertex> vertices;
    vertices.reserve(a->_vertices.size() + b->_vertices.size());
    vertices.insert(vertices.end(), a->_vertices.begin(), a->_vertices.end());
    vertices.insert(vertices.end(), b->_vertices.begin(), b->_vertices.end());

    std::vector<GLuint> indices;
    indices.reserve(a->_indices.size() + b->_indices.size());
    indices.insert(indices.end(), a->_indices.begin(), a->_indices.end());
    for (const auto& i : b->_indices)
        indices.push_back(i + a->_vertices.size());

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Mesh::Cube() {
    Vertex vertices[] = {
        Vertex{glm::vec3(-1, -1, -1), glm::vec3(0, 0, 0), glm::vec3(0,0,-1), glm::vec3(0,0,0), glm::vec2(0, 0)},
        Vertex{glm::vec3(1, -1, -1), glm::vec3(0, 0, 0), glm::vec3(0,0,-1), glm::vec3(0,0,0),glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 1, -1), glm::vec3(0, 0, 0), glm::vec3(0,0,-1), glm::vec3(0,0,0),glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 1, -1), glm::vec3(0, 0, 0), glm::vec3(0,0,-1), glm::vec3(0,0,0),glm::vec2(1, 1)},

        Vertex{glm::vec3(-1, -1, 1), glm::vec3(0, 0, 0), glm::vec3(0,0,1), glm::vec3(0,0,0),glm::vec2(0, 0)},
        Vertex{glm::vec3(1, -1, 1), glm::vec3(0, 0, 0), glm::vec3(0,0,1), glm::vec3(0,0,0),glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 1, 1), glm::vec3(0, 0, 0), glm::vec3(0,0,1), glm::vec3(0,0,0),glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), glm::vec3(0,0,1), glm::vec3(0,0,0),glm::vec2(1, 1)},

        Vertex{glm::vec3(1, -1, -1), glm::vec3(1, 1, 1), glm::vec3(1,0,0), glm::vec3(0,0,0),glm::vec2(0, 0)},
        Vertex{glm::vec3(1, -1, 1), glm::vec3(0, 1, 0), glm::vec3(1,0,0), glm::vec3(0,0,0),glm::vec2(1, 0)},
        Vertex{glm::vec3(1, 1, -1), glm::vec3(0, 0, 1), glm::vec3(1,0,0), glm::vec3(0,0,0),glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 1, 1), glm::vec3(1, 0, 0), glm::vec3(1,0,0), glm::vec3(0,0,0),glm::vec2(1, 1)},

        Vertex{glm::vec3(-1, -1, 1), glm::vec3(1, 1, 1), glm::vec3(-1,0,0), glm::vec3(0,0,0),glm::vec2(0, 0)},
        Vertex{glm::vec3(-1, -1, -1), glm::vec3(1, 0, 0), glm::vec3(-1,0,0), glm::vec3(0,0,0),glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 1, 1), glm::vec3(0, 1, 0), glm::vec3(-1,0,0), glm::vec3(0,0,0),glm::vec2(0, 1)},
        Vertex{glm::vec3(-1, 1, -1), glm::vec3(0, 0, 1), glm::vec3(-1,0,0), glm::vec3(0,0,0),glm::vec2(1, 1)},

        Vertex{glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1), glm::vec3(0,-1,0), glm::vec3(0,0,0),glm::vec2(0, 0)},
        Vertex{glm::vec3(1, -1, -1), glm::vec3(1, 0, 0), glm::vec3(0,-1,0), glm::vec3(0,0,0),glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, -1, 1), glm::vec3(0, 1, 0), glm::vec3(0,-1,0), glm::vec3(0,0,0),glm::vec2(0, 1)},
        Vertex{glm::vec3(1, -1, 1), glm::vec3(0, 0, 1), glm::vec3(0,-1,0), glm::vec3(0,0,0),glm::vec2(1, 1)},

        Vertex{glm::vec3(-1, 1, -1), glm::vec3(1, 1, 1), glm::vec3(0,1,0), glm::vec3(0,0,0),glm::vec2(0, 0)},
        Vertex{glm::vec3(1, 1, -1), glm::vec3(1, 0, 0), glm::vec3(0,1,0), glm::vec3(0,0,0),glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 1, 1), glm::vec3(0, 1, 0), glm::vec3(0,1,0), glm::vec3(0,0,0),glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 1, 1), glm::vec3(0, 0, 1), glm::vec3(0,1,0), glm::vec3(0,0,0),glm::vec2(1, 1)},
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
        16,17,19,
        16,18,19,
        20, 21, 23,
        20, 22, 23,
    };
    auto vert = std::vector(vertices, vertices + 24);
    auto ind = std::vector(indices, indices + 36);
    return std::make_shared<Mesh>(vert, ind);
}

std::shared_ptr<Mesh> Mesh::Plane(float size) {
    std::vector<Vertex> vertices = {
        Vertex{glm::vec3(-1, 0, -1) * size, glm::vec3(0, 0, 0), glm::vec3(0,1,0),glm::vec3(0,0,0),glm::vec2(0, 0)},
        Vertex{glm::vec3(1, 0, -1) * size, glm::vec3(0, 0, 0), glm::vec3(0,1,0),glm::vec3(0,0,0),glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 0, 1) * size, glm::vec3(0, 0, 0),glm::vec3(0,1,0), glm::vec3(0,0,0),glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 0, 1) * size, glm::vec3(0, 0, 0),glm::vec3(0,1,0), glm::vec3(0,0,0),glm::vec2(1, 1)}
    };

    std::vector<GLuint> indices = {
        0, 1, 3,
        0, 2, 3
    };

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Mesh::Quad() {
    std::vector<Vertex> vertices = {
        Vertex{glm::vec3(-1, -1, 0), glm::vec3(0, 0, 0), glm::vec3(0,0,0),glm::vec3(0,0,0),glm::vec2(0, 0)},
        Vertex{glm::vec3(1, -1, 0 ) , glm::vec3(0, 0, 0), glm::vec3(0,0,0),glm::vec3(0,0,0),glm::vec2(1, 0)},
        Vertex{glm::vec3(-1, 1, 0) , glm::vec3(0, 0, 0),glm::vec3(0,0,0), glm::vec3(0,0,0),glm::vec2(0, 1)},
        Vertex{glm::vec3(1, 1, 0), glm::vec3(0, 0, 0),glm::vec3(0,0,0), glm::vec3(0,0,0),glm::vec2(1, 1)}
    };

    std::vector<GLuint> indices = {
        0, 1, 3,
        0, 2, 3
    };

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Mesh::Load(std::string path) {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    std::ifstream file(path);
    std::string line;
    while (std::getline(file, line)) {
        auto columns = Utils::Split(line, " ");
        auto type = columns[0];
        if (type == "v") {
            float x = std::stof(columns[1]);
            float y = std::stof(columns[2]);
            float z = std::stof(columns[3]);
            positions.push_back(glm::vec3(x,y,z));
        }
        else if (type == "vn") {
            float x = std::stof(columns[1]);
            float y = std::stof(columns[2]);
            float z = std::stof(columns[3]);
            normals.push_back(glm::vec3(x,y,z));
        }
        else if (type == "vt") {
            float u = std::stof(columns[1]);
            float v = std::stof(columns[2]);
            uvs.push_back(glm::vec2(u,v));
        }
        else if (type == "f") {
            int base = vertices.size();
            for (int i = 1; i < columns.size(); i++) {
                auto vertexData = columns[i];
                auto indices = Utils::Split(vertexData, "/");
                auto position = std::stoi(indices[0]) - 1;
                auto uv = std::stoi(indices[1]) - 1;
                auto normal = std::stoi(indices[2]) - 1;

                Vertex vertex{
                    .position = positions[position],
                    .normal = normals[normal],
                    .UV = uvs[uv]
                };
                vertices.push_back(vertex);
            }

            for (int i = 1; i < columns.size() - 2; i++) {
                indices.push_back(base);
                indices.push_back(base + i);
                indices.push_back(base + i + 1);
            }
        }
    }
    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Mesh::Transform(std::shared_ptr<Mesh> mesh, glm::mat4 transform) {
    std::vector vertices(mesh->_vertices);

    for (auto& vertex : vertices) {
        vertex.position = xyz((transform * glm::vec4(vertex.position,1)));
    }

    return std::make_shared<Mesh>(vertices, mesh->_indices);

}
