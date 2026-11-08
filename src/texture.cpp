#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"

#include <vector>
#include <string>

GLuint Texture::LoadTexture(const char* path) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    auto *image = stbi_load(path, &width, &height, &channels,4);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);
    return texture;
}

GLuint Texture::LoadSkybox(std::string path) {
    GLuint skybox;
    glGenTextures(1, &skybox);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);

    stbi_set_flip_vertically_on_load(false);

    std::string faceFiles[] = {
        "nx.png",
        "px.png",
        "py.png",
        "ny.png",
        "nz.png",
        "pz.png",
    };

    for (int i= 0; i < 6; i++) {
        auto facePath = path + "/" + faceFiles[i];
        int width, height, nrChannels;
        auto data = stbi_load(facePath.c_str(), &width, &height, &nrChannels, 0);


        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
        );
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return skybox;
}

GLuint Texture::GenTexture(int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return texture;
}
