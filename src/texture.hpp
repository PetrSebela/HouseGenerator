#pragma once

#include <string>
#include <glad/gl.h>

// Collection of useful function regarding textures
class Texture {
    public:
    static GLuint LoadTexture(const char* path);
    static GLuint LoadSkybox(std::string path);
    static GLuint GenTexture(int width, int height);
};