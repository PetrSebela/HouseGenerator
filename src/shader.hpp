#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <glad/gl.h>

#include "glm/fwd.hpp"

/**
 * Abstraction on OpenGL programs and shader compilation
 */
class Shader {
    GLuint _program = 0;    // Program ID
    std::unordered_map<GLuint, GLuint> _textures;

    inline static GLuint bound;    // ID of program which is currently bound
    inline static std::vector<std::shared_ptr<Shader>> _instances;
    Shader(std::string vsPath, std::string fsPath);
    Shader() = default;

public:
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    ~Shader() = default;

    /**
     * Loads shader from file
     * @param path shader file path
     * @param type shader file type
     * @return Loaded and compiled shader object
     */
    static GLuint LoadShaderFromFile(std::string path, GLuint type);

    /**
     * Loads and compiles shader program
     * @param vsPath path to vertex shader
     * @param fsPath path to fragment shader
     * @return Shared pointer to shader object
     */
    static std::shared_ptr<Shader> LoadShader(std::string vsPath, std::string fsPath);

    void Discard();
    void Bind();
    void SetMatrix4x4(const char* name, glm::mat4 matrix);
    void SetTexture2D(GLuint unit, GLuint texture);
    void SetFloat(const char* name, float value);
    void SetInt(const char* name, int value);

    /**
     * Sets matrix to all shader instances
     * @warning The matrix is set ONLY to instances that were made BEFORE call to this function
     */
    static void SetGlobalMatrix4x4(const char* name, glm::mat4 matrix);
};

