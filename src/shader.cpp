#include "shader.hpp"

#include <algorithm>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>

#include "glm/gtc/type_ptr.hpp"

GLuint Shader::LoadShaderFromFile(std::string path, GLuint type) {
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

std::shared_ptr<Shader> Shader::LoadShader(std::string vsPath, std::string fsPath) {
    auto shader = std::shared_ptr<Shader>(new Shader(vsPath, fsPath));
    _instances.push_back(shader);
    return shader;
}

Shader::Shader(std::string vsPath, std::string fsPath) : Shader(){
    GLuint vertexShader = LoadShaderFromFile(vsPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = LoadShaderFromFile(fsPath, GL_FRAGMENT_SHADER);

    _program = glCreateProgram();
    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    glLinkProgram(_program);

    int success;
    char infoLog[1024];
    glGetProgramiv(_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(_program, 1024, nullptr, infoLog);
        throw std::runtime_error("( " + vsPath + "," + fsPath + " ) Shader linking failed: " + std::string(infoLog));
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::cout << "Loaded shader:" << fsPath << " | "<< _instances.size() << std::endl;
}


void Shader::Discard() {
    // _instances.erase(std::remove_if(_instances.begin(), _instances.end(), [this](Shader& shader) {
    //     return shader._program == _program;
    // }), _instances.end());
}

void Shader::Bind() {
    if (bound == _program)
        return;

    bound = _program;
    glUseProgram(_program);

    for (auto texture: _textures) {
        glActiveTexture(texture.first);
        glBindTexture(GL_TEXTURE_2D, texture.second);
    }
}

void Shader::SetMatrix4x4(const char* name, glm::mat4 matrix) {
    Bind();
    auto uniform = glGetUniformLocation(_program, name);
    glUniformMatrix4fv(uniform, 1, GL_FALSE, &matrix[0][0]);
    std::cout << "Setting matrix: " << uniform << "|"<< name << std::endl;
}

void Shader::SetTexture2D(GLuint unit, GLuint texture) {
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    _textures[unit] = texture;
}

void Shader::SetFloat(const char *name, float value) {
    Bind();
    auto uniform = glGetUniformLocation(_program, name);
    glUniform1f(uniform, value);
}

void Shader::SetInt(const char *name, int value) {
    Bind();
    auto uniform = glGetUniformLocation(_program, name);
    glUniform1i(uniform, value);
}

void Shader::SetGlobalMatrix4x4(const char *name, glm::mat4 matrix) {
    std::cout << "Global" << std::endl;
    for (auto shader : _instances) {
        shader->SetMatrix4x4(name, matrix);
    }
    std::cout << "End" << std::endl;
}
