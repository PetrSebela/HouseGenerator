#pragma once

#include <string>
#include <glad/gl.h>

#include "glm/fwd.hpp"

/**
 * Abstraction on OpenGL programs and shader compilation
 */
class Shader {
    GLuint _program = 0;    // Program ID
    static GLuint bound;    // ID of program which is currently bound

public:
    /**
     * Loads shader from file
     * @param path shader file path
     * @param type shader file type
     * @return Loaded and compiled shader object
     */
    static GLuint LoadShader(std::string path, GLuint type);

    Shader(std::string vsPath, std::string fsPath);
    Shader()=default;
    ~Shader()= default;

    void Bind();
    void SetMatrix4x4(const char* name, glm::mat4 matrix);
    void SetTexture2D(GLuint unit, GLuint texture);
    void SetFloat(const char* name, float value);
};
