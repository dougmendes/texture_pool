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

bool TexturePool::preloadTexture(const std::string& filename, const std::string& name) {
    std::lock_guard<std::mutex> lock(pool_mutex);

    // check if texture already exists
    if (name_to_index.find(name) != name_to_index.end()) {
        std::cout << "Texture " << name << " already in pool.\n";
        return true;
    }

    int width, height;
    unsigned int texture_id = loadTextureFromFile(filename, width, height);

    TextureInfo info = {
        texture_id, 
        width, height,
        std::chrono::steady_clock::now(),
        false,
        name
    };

    pool.push_back(info);
    name_to_index[name] = pool.size() - 1;

    std::cout << "Preloaded texture: " << name << " (ID: " << texture_id << ")\n";
    return true;
}