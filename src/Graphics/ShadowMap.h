#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class ShadowMap
{
public:
    ShadowMap(int width, int height);
    ~ShadowMap();
    void Bind();
    void Unbind(int screenWidth, int screenHeight);
    glm::mat4 GetLightSpaceMatrix(glm::vec3 lightPosition, glm::vec3 lightDirection);
    unsigned int GetMapID() const
    {
	return depthMapID;
    }
    int GetWidth() const
    {
	return shadowWidth;
    }
    int GetHeight() const
    {
	return shadowHeight;
    }

private:
    unsigned int fbo;
    unsigned int depthMapID;
    int shadowWidth;
    int shadowHeight;
};