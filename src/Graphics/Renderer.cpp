#include "Renderer.h"
#include "glad/glad.h"
#include "Shader.h"
#include "Core/Window.h"
#include "Core/Camera.h"
#include "Scenes/Scene.h"
#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"
#include "Resources/ResourceManager.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Development/EditorGrid.h"
Renderer::Renderer()
{
    
}

void Renderer::Render(Scene *scene, ResourceManager *resourceManager, Shader *shader, Camera *camera, Window* window, EditorGrid* editorGrid)
{

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 
    shader->Use();
    int windowHeight = window->GetHeight();
    int windowWidth = window->GetWidth();
    glm::mat4 projectionMatrix = camera->GetProjectionMatrix(float(windowWidth), float(windowHeight));
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    shader->SetMat4("projection",projectionMatrix);
    shader->SetMat4("view",viewMatrix);
    
    for(uint16_t i=0;i<scene->scenePositions.size();i++)
    {
        Mesh* mesh = resourceManager->GetMesh(scene->sceneMeshes[i]);
        Texture* texture = resourceManager->GetTexture(scene->sceneTextures[i]);
        if(mesh != nullptr && texture != nullptr)
        {
            glm::mat4 modelMatrix(1.0f);
            modelMatrix = glm::translate(modelMatrix, scene->scenePositions[i]);

            glm::quat rotationQuat = glm::quat(scene->sceneRotations[i].w, scene->sceneRotations[i].x, scene->sceneRotations[i].y, scene->sceneRotations[i].z);
            glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuat);
            modelMatrix = modelMatrix * rotationMatrix;

            modelMatrix = glm::scale(modelMatrix, scene->sceneScales[i]);
            
            shader->SetMat4("model",modelMatrix);
            texture->Use(0);
            mesh->Draw();
        }
    }
    if(editorGrid != nullptr)
    {
        editorGrid->EditorGridDraw(camera,(float)window->GetWidth(),(float)window->GetHeight());
    }
    
}

