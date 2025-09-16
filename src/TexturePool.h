#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <mutex>

struct TextureInfo {
    unsigned int id;
    int width, height;
    std::chrono::steady_clock::time_point last_used;
    bool in_use;
    std::string name;
};

class TexturePool {
private:
    std::vector<TextureInfo> pool;
    std::unordered_map<std::string, size_t> name_to_index;
    mutable std::mutex pool_mutex;

public:
    bool preloadTexture(const std::string& filename, const std::string& name);

    unsigned int acquireTexture(const std::string& name);

    void releaseTexture(const std::string& name);

    void cleanup(int max_unused_seconds = 30);

    void printStats() const;

    ~TexturePool();

};