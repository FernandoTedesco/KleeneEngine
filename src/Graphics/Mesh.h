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
    glm::vec3 boundsMin;
    glm::vec3 boundsMax;
    void Draw();
    void SetupMesh();
    void GenerateTerrain(int width, int depth);
    bool LoadOBJ(const std::filesystem::path filePath);
    void UpdateTileUVs(int tileIndex, glm::vec2 newUVs[4]);

private:
    unsigned int VAO, VBO, EBO;
};
