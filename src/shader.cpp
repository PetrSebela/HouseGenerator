#include "shader.hpp"

#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>

#include "glm/gtc/type_ptr.hpp"

GLuint Shader::bound;

GLuint Shader::LoadShader(std::string path, GLuint type) {
    if (!std::filesystem::exists(path)) {
        auto current = std::filesystem::current_path();
        std::cout << current << std::endl;
        throw std::runtime_error("No such source file: " + path);
    }

    std::ifstream vertexShaderFile(path);
    std::stringstream buffer;
    buffer << vertexShaderFile.rdbuf();

    std::string sourceString = buffer.str();
    auto source = sourceString.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        throw std::runtime_error("Shader compilation failed: " + std::string(infoLog));
    }

    glUseProgram(shader);
    bound = shader;
    return shader;
}

Shader::Shader(std::string vsPath, std::string fsPath) {
    GLuint vertexShader = LoadShader(vsPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = LoadShader(fsPath, GL_FRAGMENT_SHADER);

    _program = glCreateProgram();
    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    glLinkProgram(_program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Bind() {
    if (bound == _program)
        return;

    bound = _program;
    glUseProgram(_program);
}

void Shader::SetMatrix4x4(const char* name, glm::mat4 matrix) {
    Bind();
    auto uniform = glGetUniformLocation(_program, name);
    glUniformMatrix4fv(uniform, 1, GL_FALSE, &matrix[0][0]);
}

void Shader::SetTexture2D(GLuint unit, GLuint texture) {
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, texture);
}
