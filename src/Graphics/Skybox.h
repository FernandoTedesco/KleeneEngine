#pragma once
#include <vector>
#include <string>
#include <glad/glad.h>
#include "glm/glm.hpp"
#include "Shader.h"

class Skybox
{
public:
    Skybox(std::vector<std::string> facePaths);
    ~Skybox();
    void Draw(Shader* shader, glm::mat4 view, glm::mat4 projection);

private:
    unsigned int textureID;
    unsigned int VAO, VBO;
    void SetupMesh();
    unsigned int LoadCubemap(std::vector<std::string> faces);
};