#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Graphics/Shader.h"
#include "Core/Camera.h"
#include "Core/Window.h"
struct GameObject;
enum class GizmoMode { TRANSLATE, ROTATE, SCALE };

enum class GizmoAxis { NONE, X, Y, Z };

class Gizmo
{
public:
    Gizmo();
    ~Gizmo();
    void Draw(Camera* camera, glm::vec3 position, Shader* shader, Window* window);
    void Translate(GameObject& gameObject, float sensitivity, bool snapEnabled, float gridSize);

private:
    unsigned int VAO, VBO;
    void SetupGizmoGeometry();
};