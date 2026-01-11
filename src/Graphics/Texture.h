#pragma once
#include <string>
#include <filesystem>
class Texture
{

public:
    Texture();
    ~Texture();
    bool LoadTexture(std::filesystem::path filepath);
    unsigned int GetID() const
    {
	return this->textureID;
    }
    void Use(unsigned int slot);
    int GetWidth() const
    {
	return width;
    }
    int GetHeight() const
    {
	return height;
    }

private:
    unsigned int textureID;
    int width = 0;
    int height = 0;
};