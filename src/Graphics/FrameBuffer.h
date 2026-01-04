#pragma once
class FrameBuffer
{
public:
    FrameBuffer(int width, int height);
    ~FrameBuffer();
    unsigned int GetBrightTextureID()
    {
	return brightTextureID;
    }
    unsigned int GetDepthTextureID()
    {
	return depthTextureID;
    }
    void Bind();
    void Unbind();
    void Resize(int width, int height);
    unsigned int GetTextureID() const
    {
	return textureID;
    }

private:
    unsigned int fbo;
    unsigned int textureID;
    unsigned int brightTextureID;
    unsigned int depthTextureID;
};