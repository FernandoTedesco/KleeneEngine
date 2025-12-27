#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <filesystem>
class Mesh
{

public:
    Mesh();
    ~Mesh();
    struct Vertex {
	glm::vec3 position;
	glm::vec2 textureCoordinates;
	glm::vec3 normal;
    };
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indexes;
    void Draw();
    void SetupMesh();
    bool LoadOBJ(const std::filesystem::path filePath);

private:
    unsigned int VAO, VBO, EBO;
};
