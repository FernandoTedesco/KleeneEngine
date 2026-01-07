#include <glad/glad.h>
#include "PostProcessManager.h"
#include <iostream>
#include "Shader.h"
#include "Core/Window.h"
#include "Resources/ResourceManager.h"

PostProcessManager::PostProcessManager(Window* window)
{
    std::filesystem::path root = ResourceManager::FolderFinder("assets");

    blurShader = new Shader((root / "assets/shaders/screen.vert").string(),
			    (root / "assets/shaders/blur.frag").string());

    screenShader = new Shader((root / "assets/shaders/screen.vert").string(),
			      (root / "assets/shaders/screen.frag").string());

    InitRenderData();
    InitBuffers(window->GetWidth(), window->GetHeight());
}

PostProcessManager::~PostProcessManager()
{
    delete blurShader;
    delete screenShader;
    glDeleteVertexArrays(1, &rectVAO);
    glDeleteBuffers(1, &rectVBO);
    glDeleteFramebuffers(2, pingpongFBO);
    glDeleteTextures(2, pingpongColorbuffers);
}

void PostProcessManager::InitRenderData()
{
    float quadVertices[] = {
	-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,

	-1.0f, 1.0f, 0.0f, 1.0f, 1.0f,	-1.0f, 1.0f, 0.0f, 1.0f, 1.0f,	1.0f, 1.0f};
    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);
    glBindVertexArray(rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void PostProcessManager::InitBuffers(int width, int height)
{
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);

    for (unsigned int i = 0; i < 2; i++)
    {
	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			       pingpongColorbuffers[i], 0);
    }
}
unsigned int PostProcessManager::ExecuteBlur(unsigned int sourceTexture, int amount)
{
    bool horizontal = true, first_iteration = true;
    blurShader->Use();
    for (int i = 0; i < amount; i++)
    {
	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
	blurShader->SetInt("horizontal", horizontal);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,
		      first_iteration ? sourceTexture : pingpongColorbuffers[!horizontal]);
	glBindVertexArray(rectVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	horizontal = !horizontal;
	if (first_iteration)
	    first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return pingpongColorbuffers[!horizontal];
}
void PostProcessManager::RenderFinal(unsigned int sceneTexture, unsigned int bloomTexture)
{
    screenShader->Use();
    glBindVertexArray(rectVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    screenShader->SetInt("screenTexture", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloomTexture);
    screenShader->SetInt("bloomBlur", 1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}