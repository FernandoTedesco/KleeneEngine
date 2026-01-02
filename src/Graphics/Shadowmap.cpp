#include "ShadowMap.h"
#include <glad/glad.h>
#include <iostream>
#include "Development/Terminal.h"

ShadowMap::ShadowMap(int width, int height)
{
    this->shadowWidth = width;
    this->shadowHeight = height;
    glGenFramebuffers(1, &fbo);

    glGenTextures(1, &depthMapID);
    glBindTexture(GL_TEXTURE_2D, depthMapID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT,
		 GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapID, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
	Terminal::Log(LOG_ERROR, "ShadowMap Framebuffer is not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
ShadowMap::~ShadowMap()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &depthMapID);
}
void ShadowMap::Bind()
{
    glViewport(0, 0, shadowWidth, shadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
}
void ShadowMap::Unbind(int screenWidth, int screenHeight)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screenWidth, screenHeight);
}
glm::mat4 ShadowMap::GetLightSpaceMatrix(glm::vec3 lightPosition, glm::vec3 lightDirection)
{
    float nearPlane = 1.0f;
    float farPlane = 150.0f;
    glm::mat4 lightProjectionMatrix = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, nearPlane, farPlane);
    glm::mat4 lightViewMatrix =
	glm::lookAt(lightPosition, lightPosition + lightDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    return lightProjectionMatrix * lightViewMatrix;
}
