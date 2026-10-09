#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <glad/gl.h>

#include "glm/fwd.hpp"

class Shader {
    GLuint _program = 0;
    static GLuint bound;

public:
    static GLuint LoadShader(std::string path, GLuint type);
    Shader(std::string vsPath, std::string fsPath);
    Shader()=default;
    ~Shader()= default;

    void Bind();
    void SetMatrix4x4(const char* name, glm::mat4 matrix);
    void SetTexture2D(GLuint unit, GLuint texture);
};

#endif
