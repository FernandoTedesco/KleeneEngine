#pragma once
#include <string>
#include <filesystem>
class Texture{

    public:
    Texture();
    ~Texture();
    bool LoadTexture(std::filesystem::path filepath);
    void Use(unsigned int slot);

    private:
    unsigned int textureID;



};