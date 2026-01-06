#include <glad/glad.h>
#include "Mesh.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <filesystem>
Mesh::Mesh()
{
    VAO = 0;
    VBO = 0;
    EBO = 0;
    this->boundsMin = glm::vec3(-0.5f, -0.5f, -0.5f);
    this->boundsMax = glm::vec3(0.5f, 0.5f, 0.5f);
}

void Mesh::SetupMesh()
{

    // Configuration for data delivery
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Data delivery RAM->VRAM
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertices.size(), this->vertices.data(),
		 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			  (void*)offsetof(Mesh::Vertex, textureCoordinates));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			  (void*)offsetof(Mesh::Vertex, normal));

    // EBO Initialization
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->indexes[0]) * this->indexes.size(),
		 this->indexes.data(), GL_STATIC_DRAW);

    // VAO detachment
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::Draw()
{

    // Activate VAO stage
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->indexes.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void Mesh::UpdateTileUVs(int tileIndex, glm::vec2 newUVs[4])
{
    int startIndex = tileIndex * 4;
    if (startIndex < 0 || startIndex + 3 >= (int)this->vertices.size())
	return;
    this->vertices[startIndex + 0].textureCoordinates = newUVs[0];
    this->vertices[startIndex + 1].textureCoordinates = newUVs[1];
    this->vertices[startIndex + 2].textureCoordinates = newUVs[2];
    this->vertices[startIndex + 3].textureCoordinates = newUVs[3];
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    GLintptr offset = startIndex * sizeof(Vertex);
    GLsizeiptr size = 4 * sizeof(Vertex);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, &this->vertices[startIndex]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void Mesh::GenerateTerrain(int width, int depth)
{
    this->vertices.clear();
    this->indexes.clear();
    this->boundsMin = glm::vec3(0.0f);
    this->boundsMax = glm::vec3((float)width, 0.0f, (float)depth);
    int vertexCount = 0;

    for (int z = 0; z <= depth; z++)
    {
	for (int x = 0; x < width; x++)
	{
	    float x0 = (float)x;
	    float z0 = (float)z;
	    float x1 = (float)x + 1.0f;
	    float z1 = (float)z + 1.0f;
	    Vertex v0;
	    v0.position = glm::vec3((float)x, 0.0f, (float)z);
	    v0.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	    v0.textureCoordinates = glm::vec2(0.0f, 0.0f);
	    this->vertices.push_back(v0);

	    Vertex v1;
	    v1.position = glm::vec3(x0, 0.0f, z1);
	    v1.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	    v1.textureCoordinates = glm::vec2(0.0f, 1.0f);
	    this->vertices.push_back(v1);

	    Vertex v2;
	    v2.position = glm::vec3(x1, 0.0f, z1);
	    v2.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	    v2.textureCoordinates = glm::vec2(1.0f, 1.0f);
	    this->vertices.push_back(v2);

	    Vertex v3;
	    v3.position = glm::vec3(x1, 0.0f, z0);
	    v3.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	    v3.textureCoordinates = glm::vec2(1.0f, 0.0f);
	    this->vertices.push_back(v3);

	    this->indexes.push_back(vertexCount + 0);
	    this->indexes.push_back(vertexCount + 1);
	    this->indexes.push_back(vertexCount + 2);

	    this->indexes.push_back(vertexCount + 0);
	    this->indexes.push_back(vertexCount + 2);
	    this->indexes.push_back(vertexCount + 3);

	    vertexCount += 4;
	}
    }
}
bool Mesh::LoadOBJ(const std::filesystem::path filePath)
{

    std::ifstream objFile;

    std::vector<glm::vec3> positionContainer;
    std::vector<glm::vec2> textureCoordinateContainer;
    std::vector<glm::vec3> normalContainer;
    std::string line;

    objFile.open(filePath.string());
    if (objFile.is_open())
    {

	while (std::getline(objFile, line))
	{

	    std::stringstream objStream(line);
	    std::string prefix;
	    objStream >> prefix;

	    if (prefix == "v")
	    {

		// Vertex Reader
		glm::vec3 pos;
		objStream >> pos.x;
		objStream >> pos.y;
		objStream >> pos.z;

		positionContainer.push_back(pos);

	    } else if (prefix == "vt")
	    {
		glm::vec2 uv;
		objStream >> uv.x;
		objStream >> uv.y;
		textureCoordinateContainer.push_back(uv);

	    } else if (prefix == "vn")
	    {
		glm::vec3 normal;
		objStream >> normal.x;
		objStream >> normal.y;
		objStream >> normal.z;
		normalContainer.push_back(normal);
	    } else if (prefix == "f")
	    {

		// Main unifier of mesh data
		unsigned int posIndex, uvIndex, normalIndex;
		char slash;
		for (int i = 0; i < 3; i++)
		{
		    objStream >> posIndex >> slash >> uvIndex >> slash >> normalIndex;
		    Vertex vertex;
		    vertex.position = positionContainer[posIndex - 1];
		    vertex.textureCoordinates = textureCoordinateContainer[uvIndex - 1];
		    vertex.normal = normalContainer[normalIndex - 1];

		    this->vertices.push_back(vertex);
		    this->indexes.push_back(this->vertices.size() - 1);
		}
	    }
	}
	this->boundsMin = glm::vec3(FLT_MAX);
	this->boundsMax = glm::vec3(-FLT_MAX);
	for (size_t i = 0; i < this->vertices.size(); i++)
	{
	    glm::vec3 currentPosition = this->vertices[i].position;
	    if (currentPosition.x < this->boundsMin.x)
	    {
		this->boundsMin.x = currentPosition.x;
	    }
	    if (currentPosition.y < this->boundsMin.y)
	    {
		this->boundsMin.y = currentPosition.y;
	    }
	    if (currentPosition.z < this->boundsMin.z)
	    {
		this->boundsMin.z = currentPosition.z;
	    }
	    if (currentPosition.x > this->boundsMax.x)
	    {
		this->boundsMax.x = currentPosition.x;
	    }
	    if (currentPosition.y > this->boundsMax.y)
	    {
		this->boundsMax.y = currentPosition.y;
	    }
	    if (currentPosition.z > this->boundsMax.z)
	    {
		this->boundsMax.z = currentPosition.z;
	    }
	}

	objFile.close();
	return (true);
    } else
    {

	return false;
    }
}

Mesh::~Mesh()
{

    if (VAO != 0)
    {
	glDeleteVertexArrays(1, &VAO);
	VAO = 0;
    }
    if (VBO != 0)
    {
	glDeleteBuffers(1, &VBO);
	VBO = 0;
    }
    if (EBO != 0)
    {
	glDeleteBuffers(1, &EBO);
	EBO = 0;
    }
}