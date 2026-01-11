#include "Skybox.h"
#include <glad/glad.h>
#include <iostream>
#include "StbImage/stb_image.h"
#include "Development/Terminal.h"
Skybox::Skybox(std::vector<std::string> facePaths)
{
    this->storedPaths = facePaths;
    SetupMesh();
    textureID = LoadCubemap(facePaths);
}
Skybox::~Skybox()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &textureID);
}

void Skybox::Draw(Shader* shader, glm::mat4 view, glm::mat4 projection)
{

    shader->Use();
    int locView = glGetUniformLocation(shader->GetID(), "view");
    int locProj = glGetUniformLocation(shader->GetID(), "projection");
    if (locView == -1 || locProj == -1)
    {
	Terminal::Log(LOG_ERROR, "Skybox broken");
    }
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
    shader->SetMat4("view", viewNoTranslation);
    shader->SetMat4("projection", projection);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    shader->SetInt("skybox", 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
void Skybox::SetupMesh()
{
    float skyboxVertices[] = {-1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,
			      -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

			      -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
			      1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,	 -1.0f, -1.0f, 1.0f,

			      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,	1.0f,  1.0f,  1.0f,  1.0f,
			      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,	-1.0f, -1.0f,

			      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,	 1.0f,	1.0f,  1.0f,  1.0f,  1.0f,
			      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,	 -1.0f, -1.0f, 1.0f,

			      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,	 -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
			      1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,	 -1.0f, 1.0f,  -1.0f,

			      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,	1.0f,  -1.0f, -1.0f, 1.0f,
			      -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,	 1.0f,	-1.0f, 1.0f

    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

unsigned int Skybox::LoadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    stbi_set_flip_vertically_on_load(false);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
	Terminal::Log(LOG_INFO, "Tried to Load: " + faces[i]);
	unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 3);
	if (data)
	{
	    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB,
			 GL_UNSIGNED_BYTE, data);
	    Terminal::Log(LOG_SUCCESS, "Face loaded, releasing RAM ");
	    stbi_image_free(data);
	} else
	{
	    Terminal::Log(LOG_ERROR, "Cubemap texture failed to load");
	    stbi_image_free(data);
	}
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}