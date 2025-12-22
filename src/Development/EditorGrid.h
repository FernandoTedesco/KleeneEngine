#pragma once
#include <vector>
#include <glm/glm.hpp>

class Shader;
class Camera;

class EditorGrid
{
    public:
    EditorGrid(int size);
    void EditorGridDraw(Camera *camera);
    ~EditorGrid();

    

    private:
    unsigned int VAO;
    unsigned int VBO;
    Shader* shader;
    int vertexCount;
    std::vector<float> gridVertices;
};