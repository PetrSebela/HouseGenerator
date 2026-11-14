#pragma once

#include <string>
#include <glad/gl.h>

// TODO: Change this setup
// This tetup will lead to memory leaks for sure, as I am sure
// I will forget to release texture memory after i amd done using the texture :D

// Collection of useful function regarding textures
class Texture {
    public:
    static GLuint LoadTexture(const char* path);
    static GLuint LoadSkybox(std::string path);
    static GLuint GenTexture(int width, int height);
};