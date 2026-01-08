#pragma once
#include <vector>
#include <glm/glm.hpp>
class Scene;
class ResourceManager;
class Shader;
class Camera;
class Window;
class EditorGrid;
class MeshRenderer;
class Light;
class Editor;
class ShadowMap;
class FrameBuffer;
class PostProcessManager;

class Renderer
{
public:
    Renderer(Window* window);
    ~Renderer();
    Shader* GetMainShader() const
    {
	return mainShader;
    }
    Shader* GetParticleShader() const
    {
	return particleShader;
    }
    Shader* GetShadowShader() const
    {
	return shadowShader;
    }

    void RenderFrame(Scene* scene, ResourceManager* resourceManager, Shader* shader, Camera* camera,
		     Window* window, Editor* editor, bool isShadowPass = false);

private:
    ShadowMap* shadowMap;
    FrameBuffer* frameBuffer;
    PostProcessManager* postProcessManager;
    ResourceManager* resourceManager;
    Shader* mainShader;
    Shader* shadowShader;
    Shader* skyboxShader;
    Shader* particleShader;
    void PassShadowMap(Scene* scene, Camera* camera, Window* window,
		       ResourceManager* resourceManager);
    void PassColor(Scene* scene, Camera* camera, Window* window, Editor* editor,
		   ResourceManager* resourceManager);
    void PassPostProcess();

    void PrepareScene(Camera* camera, Shader* shader, Window* window);
    void SetupLights(Scene* scene, Shader* shader);
    void DrawMesh(Scene* scene, MeshRenderer* meshRenderer, ResourceManager* ResourceManager,
		  Shader* shader, bool isShadowPass);
};