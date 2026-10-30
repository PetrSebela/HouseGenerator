#pragma once

#include <glad/gl.h>

// Collection of useful function regarding textures
class Texture {
    public:
    static GLuint LoadTexture(const char* path);
    static GLuint GenTexture(int width, int height);
};