#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Graphics/Shader.h"
#include "Core/Camera.h"
#include "Core/Window.h"
class GameObject;
enum class GizmoMode { TRANSLATE, ROTATE, SCALE };

enum class GizmoAxis { NONE, X, Y, Z };

class Gizmo
{
public:
    Gizmo();
    ~Gizmo();
    void Draw(Camera* camera, glm::vec3 position, Shader* shader, Window* window);
    void DrawLightIcon(Camera* camera, glm::vec3 position, Shader* shader, Window* window);
    void SetupLightIconGeometry();
    GizmoAxis CheckHover(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 objectPosition,
			 float scale);
    void Translate(GameObject& gameObject, float sensitivity, bool snapEnabled, float gridSize);

private:
    unsigned int VAO, VBO;
    unsigned int lightVAO, lightVBO;
    void SetupGizmoGeometry();
};