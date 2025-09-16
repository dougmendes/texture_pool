// TexturePool.cpp
#include "TexturePool.h"
#include <GL/gl.h>
#include <iostream>
#include <algorithm>

// Função auxiliar para carregar PNG (simplified)
unsigned int loadTextureFromFile(const std::string& filename, int& width, int& height) {
    // Aqui você usaria STBI ou similar
    // Para demo, vamos simular:
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Simula load de arquivo
    width = 256; height = 256;
    std::vector<unsigned char> dummy_data(width * height * 4, 255);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, 
                 GL_RGBA, GL_UNSIGNED_BYTE, dummy_data.data());
    
    std::cout << "Loaded texture: " << filename << " (simulated)\n";
    return textureID;
}

bool TexturePool::preloadTexture(const std::string& filename, const std::string& name) {
    std::lock_guard<std::mutex> lock(pool_mutex);
    
    // Verifica se já existe
    if (name_to_index.find(name) != name_to_index.end()) {
        std::cout << "Texture " << name << " already in pool\n";
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

unsigned int TexturePool::acquireTexture(const std::string& name) {
    std::lock_guard<std::mutex> lock(pool_mutex);
    
    auto it = name_to_index.find(name);
    if (it == name_to_index.end()) {
        std::cout << "ERROR: Texture " << name << " not found in pool!\n";
        return 0;
    }
    
    size_t index = it->second;
    pool[index].in_use = true;
    pool[index].last_used = std::chrono::steady_clock::now();
    
    std::cout << "Acquired texture: " << name << " (ID: " << pool[index].id << ")\n";
    return pool[index].id;
}

void TexturePool::releaseTexture(const std::string& name) {
    std::lock_guard<std::mutex> lock(pool_mutex);
    
    auto it = name_to_index.find(name);
    if (it != name_to_index.end()) {
        size_t index = it->second;
        pool[index].in_use = false;
        pool[index].last_used = std::chrono::steady_clock::now();
        std::cout << "Released texture: " << name << "\n";
    }
}

void TexturePool::cleanup(int max_unused_seconds) {
    std::lock_guard<std::mutex> lock(pool_mutex);
    auto now = std::chrono::steady_clock::now();
    
    for (auto it = pool.begin(); it != pool.end();) {
        auto time_diff = std::chrono::duration_cast<std::chrono::seconds>(
            now - it->last_used).count();
            
        if (!it->in_use && time_diff > max_unused_seconds) {
            std::cout << "Cleaning up unused texture: " << it->name << "\n";
            glDeleteTextures(1, &it->id);
            
            // Remove do mapa
            name_to_index.erase(it->name);
            
            it = pool.erase(it);
        } else {
            ++it;
        }
    }
}

void TexturePool::printStats() const {
    std::lock_guard<std::mutex> lock(pool_mutex);
    
    int in_use_count = 0;
    int total_memory = 0;
    
    for (const auto& tex : pool) {
        if (tex.in_use) in_use_count++;
        total_memory += tex.width * tex.height * 4; // RGBA
    }
    
    std::cout << "\n=== Texture Pool Stats ===\n";
    std::cout << "Total textures: " << pool.size() << "\n";
    std::cout << "In use: " << in_use_count << "\n";
    std::cout << "Available: " << (pool.size() - in_use_count) << "\n";
    std::cout << "Total memory: " << total_memory / 1024 << " KB\n";
    std::cout << "========================\n\n";
}