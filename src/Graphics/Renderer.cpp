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
#include "Graphics/Material.h"
#include "Gameplay/GameObject.h"
#include <string>
Renderer::Renderer()
{
}

void Renderer::Render(Scene* scene, ResourceManager* resourceManager, Shader* shader,
		      Camera* camera, Window* window, EditorGrid* editorGrid, bool isShadowPass)
{

    if (!isShadowPass)
    {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader->Use();

	int windowHeight = window->GetHeight();
	int windowWidth = window->GetWidth();
	glm::mat4 projectionMatrix =
	    camera->GetProjectionMatrix(float(windowWidth), float(windowHeight));
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	shader->SetMat4("projection", projectionMatrix);
	shader->SetMat4("view", viewMatrix);
	shader->SetVec3("viewPos", camera->GetCameraPos());
    }

    unsigned int lightIndex = 0;
    if (!isShadowPass)
    {
	for (const Light& light : scene->lights)
	{
	    std::string number = std::to_string(lightIndex);
	    shader->SetInt("lights[" + number + "].type", (int)light.type);
	    shader->SetVec3("lights[" + number + "].position", light.position);
	    shader->SetVec3("lights[" + number + "].direction", light.direction);
	    shader->SetVec3("lights[" + number + "].color", light.color);
	    shader->SetFloat("lights[" + number + "].intensity", light.intensity);

	    shader->SetFloat("lights[" + number + "].constant", light.constant);
	    shader->SetFloat("lights[" + number + "].linear", light.linear);
	    shader->SetFloat("lights[" + number + "].quadratic", light.quadratic);

	    shader->SetFloat("lights[" + number + "].cutOff", light.cutOff);
	    shader->SetFloat("lights[" + number + "].outerCutOff", light.outerCutOff);
	    lightIndex++;
	    if (lightIndex >= 16)
		break;
	}
	shader->SetInt("numLights", lightIndex);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (GameObject& object : scene->gameObjects)
    {
	if (object.isActive)
	{
	    Mesh* mesh = resourceManager->GetMesh(object.meshID);
	    Material* material = resourceManager->GetMaterial(object.materialID);
	    if (mesh != nullptr)
	    {
		if (!isShadowPass)
		{
		    if (material != nullptr)
		    {
			material->Use(shader);
			shader->SetFloat("material.specular", material->specular);
			shader->SetFloat("material.shininess", material->shininess);
		    } else
		    {
			shader->SetVec3("material.color", glm::vec3(1.0f, 0.0f, 1.0f));
			shader->SetVec2("material.tiling", glm::vec2(1.0f, 1.0f));
			shader->SetFloat("material.specular", 0.5f);
			shader->SetFloat("material.shininess", 32.0f);
		    }
		}
		glm::mat4 modelMatrix(1.0f);
		modelMatrix = glm::translate(modelMatrix, object.position);

		modelMatrix =
		    glm::rotate(modelMatrix, glm::radians(object.rotation.x), glm::vec3(1, 0, 0));
		modelMatrix =
		    glm::rotate(modelMatrix, glm::radians(object.rotation.y), glm::vec3(0, 1, 0));
		modelMatrix =
		    glm::rotate(modelMatrix, glm::radians(object.rotation.z), glm::vec3(0, 0, 1));

		modelMatrix = glm::scale(modelMatrix, object.scale);
		shader->SetMat4("model", modelMatrix);
		mesh->Draw();
	    }
	}
    }
    if (editorGrid != nullptr && !isShadowPass)
    {
	editorGrid->EditorGridDraw(camera, (float)window->GetWidth(), (float)window->GetHeight());
    }
}
