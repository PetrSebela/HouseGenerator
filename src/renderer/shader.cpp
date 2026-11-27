#include "shader.hpp"

#include <algorithm>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>

#include "texture.hpp"
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

std::shared_ptr<Shader> Shader::LoadShader(std::string vsPath, std::string fsPath, std::string name) {
    auto shader = std::shared_ptr<Shader>(new Shader(vsPath, fsPath, name));
    _loaded[name] = shader;
    _instances.push_back(shader);
    return shader;
}

std::shared_ptr<Shader> Shader::LoadShader(std::string vsPath, std::string fsPath) {
    auto shader = std::shared_ptr<Shader>(new Shader(vsPath, fsPath));
    _instances.push_back(shader);
    return shader;
}

Shader::Shader(std::string vsPath, std::string fsPath, std::string name) : Shader(){
    _name = name;
    _vsPath = vsPath;
    _fsPath = fsPath;

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
}

void Shader::SetMatrix4x4Current(const char *name, glm::mat4 matrix) {
    auto uniform = glGetUniformLocation(_program, name);
    glUniformMatrix4fv(uniform, 1, GL_FALSE, &matrix[0][0]);
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
    for (const auto& shader: _instances) {
        shader->SetMatrix4x4(name, matrix);
    }
}

std::shared_ptr<Shader> Shader::GetShaderByName(std::string name) {
    auto shader = _loaded.find(name);
    if (shader == _loaded.end()) {
        // TODO: default checkerboard shader
        return nullptr;
    }
    return shader->second;
}

void Shader::SetupShaders() {
    auto skyboxTexture = Texture::LoadSkybox("assets/textures/skybox");
    auto skyboxShader = Shader::LoadShader("assets/shaders/default.vert", "assets/shaders/skybox.frag", "skybox");
    skyboxShader->SetTexture2D(GL_TEXTURE0, skyboxTexture);

    LoadShader("assets/shaders/default.vert", "assets/shaders/floor.frag", "floor");

    auto plasterAlbedo = Texture::LoadTexture("assets/textures/plaster/albedo.jpg");
    auto plasterNormal = Texture::LoadTexture("assets/textures/plaster/normal.jpg");
    auto wallShader = Shader::LoadShader("assets/shaders/default.vert", "assets/shaders/wall.frag", "wall");
    wallShader->SetTexture2D(GL_TEXTURE1, plasterAlbedo);
    wallShader->SetTexture2D(GL_TEXTURE2, plasterNormal);

    auto concreteAlbedo = Texture::LoadTexture("assets/textures/concrete/albedo.jpg");
    auto concreteNormal = Texture::LoadTexture("assets/textures/concrete/normal.jpg");
    auto foundationShader = Shader::LoadShader("assets/shaders/default.vert", "assets/shaders/wall.frag", "foundation");
    foundationShader->SetTexture2D(GL_TEXTURE1, concreteAlbedo);
    foundationShader->SetTexture2D(GL_TEXTURE2, concreteNormal);


    auto basaltAlbedo = Texture::LoadTexture("assets/textures/basalt/albedo.jpg");
    auto basaltNormal = Texture::LoadTexture("assets/textures/basalt/normal.jpg");
    auto basaltShader = Shader::LoadShader("assets/shaders/default.vert", "assets/shaders/wall.frag", "basalt");
    basaltShader->SetTexture2D(GL_TEXTURE1, basaltAlbedo);
    basaltShader->SetTexture2D(GL_TEXTURE2, basaltNormal);


    auto tilesAlbedo = Texture::LoadTexture("assets/textures/tiles/albedo.jpg");
    auto tilesNormal = Texture::LoadTexture("assets/textures/tiles/normal.jpg");
    auto tilesShader = Shader::LoadShader("assets/shaders/default.vert", "assets/shaders/wall.frag", "tiles");
    tilesShader->SetTexture2D(GL_TEXTURE1, tilesAlbedo);
    tilesShader->SetTexture2D(GL_TEXTURE2, tilesNormal);
}

void Shader::ListLoadedShaders() {
    std::cout << "Total shader count: " << _loaded.size() << std::endl;
    std::cout << "Name | VS | FS" << std::endl;
    for (const auto& [name, shader] : _loaded) {
        std::cout << name << " | ";
        std::cout << shader->_vsPath << " | ";
        std::cout << shader->_fsPath << std::endl;
    }
}
