#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_
#include <string>
#include <glad/gl.h>

class Texture {
    public:
    static GLuint LoadTexture(const char* path);
};

#endif
