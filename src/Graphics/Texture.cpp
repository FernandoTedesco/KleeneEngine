#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include <glm/glm.hpp>
#include "StbImage/stb_image.h"
#include <string>
#include <exception>
#include <iostream>
#include <glad/glad.h>

Texture::Texture()
{
    this->width = 0;
    this->height = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    unsigned char data[]{// Pink and black placeholder texture
			 255, 0, 255, 0, 0, 0, 0, 0, 0, 255, 0, 255};

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

bool Texture::LoadTexture(std::filesystem::path filePath)
{

    int component;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data =
	stbi_load(((filePath).string()).c_str(), &this->width, &this->height, &component, 4);
    if (data)
    {

	glBindTexture(GL_TEXTURE_2D, textureID); // Binds it for changes

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	return true;
    } else
    {
	std::cout << "ERROR::TEXTURE::TEXTURE_LOADING_FAIL" << std::endl;
	std::cout << "Tried to open" << filePath << " but failed" << std::endl;
	return false;
    }
}

void Texture::Use(unsigned int slot)
{

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

Texture::~Texture()
{
    glDeleteTextures(1, &textureID);
}