#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"

GLuint Texture::LoadTexture(const char* path) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    auto *image = stbi_load(path, &width, &height, &channels,4);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
    return texture;
}
