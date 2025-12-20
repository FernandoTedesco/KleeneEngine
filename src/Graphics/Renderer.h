#pragma once
class Scene;
class ResourceManager;
class Shader;
class Camera;
class Window;

class Renderer
{
    public:
    Renderer();
    void Render(Scene *scene, ResourceManager *resourceManager, Shader *shader, Camera *camera, Window* window);




};