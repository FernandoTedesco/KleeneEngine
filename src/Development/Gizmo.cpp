#include "Gizmo.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "imgui.h"
#include "Scenes/GameObject.h"
#include "Utils/Math.h"
Gizmo::Gizmo()
{
    SetupGizmoGeometry();
    SetupLightIconGeometry();
}

Gizmo::~Gizmo()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);
}

void Gizmo::SetupGizmoGeometry()
{
    float vertices[] = {// X axis
			0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			// Y axis
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			// Z axis
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Gizmo::Draw(Camera* camera, glm::vec3 position, Shader* shader, Window* window)
{
    shader->Use();
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection =
	camera->GetProjectionMatrix((float)window->GetWidth(), (float)window->GetHeight());

    shader->SetMat4("view", view);
    shader->SetMat4("projection", projection);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    shader->SetMat4("model", model);

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(VAO);
    glLineWidth(4.0f);

    shader->SetVec3("material.color", glm::vec3(0.0f, 0.0f, 1.0f));
    glDrawArrays(GL_LINES, 0, 2);
    shader->SetVec3("material.color", glm::vec3(0.0f, 1.0f, 0.0f));
    glDrawArrays(GL_LINES, 2, 2);
    shader->SetVec3("material.color", glm::vec3(1.0f, 0.0f, 0.0f));
    glDrawArrays(GL_LINES, 4, 2);

    glBindVertexArray(0);
    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
}

void Gizmo::Translate(GameObject& gameObject, float sensitivity, bool snapEnabled, float gridSize)
{ // could have used the input variation but didnt want possible coupling issues
    float mouseDX = ImGui::GetIO().MouseDelta.x;
    float mouseDY = ImGui::GetIO().MouseDelta.y;
    if (mouseDX == 0.0F && mouseDY == 0.0f)
	return;

    gameObject.position.x += mouseDX * sensitivity;
    gameObject.position.z += mouseDY * sensitivity;
    if (snapEnabled && gridSize > 0.0f)
    {
	gameObject.position.x = std::round(gameObject.position.x / gridSize) * gridSize;
	gameObject.position.z = std::round(gameObject.position.z / gridSize) * gridSize;
    }
}

GizmoAxis Gizmo::CheckHover(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 objectPosition,
			    float scale)
{
    // hitboxes
    float thick = 0.3f * scale;
    float len = 1.5f * scale; // width
    float offset = 0.1f * scale;
    glm::vec3 minX = objectPosition + glm::vec3(offset, -thick / 2, -thick / 2);
    glm::vec3 maxX = objectPosition + glm::vec3(len, thick / 2, thick / 2);

    glm::vec3 minY = objectPosition + glm::vec3(-thick / 2, offset, -thick / 2);
    glm::vec3 maxY = objectPosition + glm::vec3(thick / 2, len, thick / 2);

    glm::vec3 minZ = objectPosition + glm::vec3(-thick / 2, -thick / 2, offset);
    glm::vec3 maxZ = objectPosition + glm::vec3(thick / 2, thick / 2, len);

    float distance;
    if (Math::RayAABBIntersection(rayOrigin, rayDirection, minX, maxX, distance))
    {
	return GizmoAxis::X;
    }
    if (Math::RayAABBIntersection(rayOrigin, rayDirection, minY, maxY, distance))
    {
	return GizmoAxis::Y;
    }
    if (Math::RayAABBIntersection(rayOrigin, rayDirection, minZ, maxZ, distance))
    {
	return GizmoAxis::Z;
    } else
    {
	return GizmoAxis::NONE;
    }
}

void Gizmo::DrawLightIcon(Camera* camera, glm::vec3 position, Shader* shader, Window* window)
{
    shader->Use();
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection =
	camera->GetProjectionMatrix((float)window->GetWidth(), (float)window->GetHeight());
    shader->SetMat4("view", view);
    shader->SetMat4("projection", projection);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    shader->SetMat4("model", model);
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(lightVAO);
    glLineWidth(2.0f);
    shader->SetVec3("material.color", glm::vec3(1.0f, 1.0f, 0.0f));
    glDrawArrays(GL_LINES, 0, 24);
    glBindVertexArray(0);
    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
}
void Gizmo::SetupLightIconGeometry()
{
    float scale = 0.5f;
    float vertices[] = {
	0.0f,	scale,	0.0f, scale, 0.0f, 0.0f,   0.0f, scale,	 0.0f,	 -scale, 0.0f, 0.0f,
	0.0f,	scale,	0.0f, 0.0f,  0.0f, scale,  0.0f, scale,	 0.0f,	 0.0f,	 0.0f, -scale,

	0.0f,	-scale, 0.0f, scale, 0.0f, 0.0f,   0.0f, -scale, 0.0f,	 -scale, 0.0f, 0.0f,
	0.0f,	-scale, 0.0f, 0.0f,  0.0f, scale,  0.0f, -scale, 0.0f,	 0.0f,	 0.0f, -scale,

	scale,	0.0f,	0.0f, 0.0f,  0.0f, scale,  0.0f, 0.0f,	 scale,	 -scale, 0.0f, 0.0f,
	-scale, 0.0f,	0.0f, 0.0f,  0.0f, -scale, 0.0f, 0.0f,	 -scale, scale,	 0.0f, 0.0f};
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}