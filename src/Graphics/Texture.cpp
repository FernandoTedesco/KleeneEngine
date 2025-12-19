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
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    unsigned char data[]{ //Pink and black placeholder texture
        255,0,255, 0,0,0,
        0,0,0, 255,0,255
    };

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2,0,GL_RGB, GL_UNSIGNED_BYTE, data);
    
}

bool Texture::LoadTexture(std::filesystem::path filePath)
{
    int width; 
    int height;
    int component;
    
    
    unsigned char* data = stbi_load(((filePath).string()).c_str(), &width, &height, &component, 0);
    if(data)
    {
        
        
        glBindTexture(GL_TEXTURE_2D, textureID); //Binds it for changes
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);

       

        GLenum format;
        if(component == 1)
        {
            format = GL_RED;
        }
        else if(component == 3)
        { 
            format = GL_RGB;
        }
        else
        {
            format = GL_RGBA;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, width,height,0,format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);

        return true;
    }
    else
    {
        std::cout<<"ERROR::TEXTURE::TEXTURE_LOADING_FAIL"<<std::endl;
        std::cout<<"Tried to open"<< filePath << " but failed" <<std::endl;
        return false;
    }



}


void Texture::Use(unsigned int slot){

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
}

Texture::~Texture(){
    glDeleteTextures(1,&textureID);
}