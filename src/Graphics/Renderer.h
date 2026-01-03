#pragma once
class Scene;
class ResourceManager;
class Shader;
class Camera;
class Window;
class EditorGrid;
class Renderer
{
public:
    Renderer();
    void Render(Scene* scene, ResourceManager* resourceManager, Shader* shader, Camera* camera,
		Window* window, EditorGrid* editorGrid, bool isShadowPass = false);
};