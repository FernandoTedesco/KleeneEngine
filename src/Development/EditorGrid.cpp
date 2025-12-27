#include "EditorGrid.h"
#include "Graphics/Shader.h"
#include <filesystem>
#include "Resources/ResourceManager.h"
#include <glad/glad.h>
#include "Core/Camera.h"
EditorGrid::EditorGrid(int size)
{
    std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
    shader = new Shader((currentPath / "assets/shaders/grid.vert").string(),
			(currentPath / "assets/shaders/grid.frag").string());

    for (int i = -size; i <= size; i++)
    {
	gridVertices.push_back((float)i);
	gridVertices.push_back((0.0f));
	gridVertices.push_back((float)-size);

	gridVertices.push_back((float)i);
	gridVertices.push_back((0.0f));
	gridVertices.push_back((float)size);

	gridVertices.push_back((float)-size);
	gridVertices.push_back((0.0f));
	gridVertices.push_back((float)i);

	gridVertices.push_back((float)size);
	gridVertices.push_back((0.0f));
	gridVertices.push_back((float)i);
    }

    vertexCount = gridVertices.size() / 3;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(),
		 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void EditorGrid::EditorGridDraw(Camera* camera, float screenWidth, float screenHeight)
{
    shader->Use();
    shader->SetMat4("view", camera->GetViewMatrix());
    shader->SetMat4("projection", camera->GetProjectionMatrix(screenWidth, screenHeight));
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, vertexCount);
    glBindVertexArray(0);
}

EditorGrid::~EditorGrid()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete shader;
}
