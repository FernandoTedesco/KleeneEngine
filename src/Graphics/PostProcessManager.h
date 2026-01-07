#pragma once
#include <string>
class Shader;
class Window;
class FrameBuffer;

class PostProcessManager
{
public:
    PostProcessManager(Window* window);
    ~PostProcessManager();
    unsigned int ExecuteBlur(unsigned int sourceTexture, int amount = 10);
    void RenderFinal(unsigned int sceneTexture, unsigned int bloomTexture);

private:
    Shader* screenShader;
    Shader* blurShader;
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];

    unsigned int rectVAO;
    unsigned int rectVBO;

    void InitRenderData();
    void InitBuffers(int width, int height);
};