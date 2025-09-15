#include "TexturePool.h"
#include <GL/gl.h>
#include <iostream>
#include <algorithm>

//aux function to load texture from file
unsigned int loadTextureFromFile(const std::string& filename, int& width, int& height){
    // Here we should use STBI or similar to load the texture
    // For this demo we will just simulate loading
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Simulate texture loading
    width = 256; height = 256; // Dummy dimensions
    std::vector<unsigned char> dummyData(width * height * 4, 255);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dummyData.data());

    std::cout << "Loaded texture: " << filename << " (simulated)\n";
    return textureID;
}