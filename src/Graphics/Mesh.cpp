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
    if (VAO == 0)
    {
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);
    }
    // Configuration for data delivery

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
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			  (void*)offsetof(Mesh::Vertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			  (void*)offsetof(Mesh::Vertex, bitangent));
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

void Mesh::CalculateTangents()
{
    if (indexes.empty())
	return;
    for (auto& v : vertices)
    {
	v.tangent = glm::vec3(0.0f);
	v.bitangent = glm::vec3(0.0f);
    }
    for (size_t i = 0; i < indexes.size(); i += 3)
    {
	unsigned int i1 = indexes[i];
	unsigned int i2 = indexes[i + 1];
	unsigned int i3 = indexes[i + 2];

	Vertex& v1 = vertices[i1];
	Vertex& v2 = vertices[i2];
	Vertex& v3 = vertices[i3];

	glm::vec3 edge1 = v2.position - v1.position;
	glm::vec3 edge2 = v3.position - v1.position;
	glm::vec2 deltaUV1 = v2.textureCoordinates - v1.textureCoordinates;
	glm::vec2 deltaUV2 = v3.textureCoordinates - v1.textureCoordinates;
	float det = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	if (std::abs(det) < 0.000001f)
	    continue;

	float f = 1.0f / det;

	glm::vec3 tangent, bitangent;

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

	v1.tangent += tangent;
	v2.tangent += tangent;
	v3.tangent += tangent;

	v1.bitangent += bitangent;
	v2.bitangent += bitangent;
	v3.bitangent += bitangent;
    }
    for (auto& v : vertices)
    {
	if (glm::length(v.tangent) > 0.0f)
	    v.tangent = glm::normalize(v.tangent);
	if (glm::length(v.bitangent) > 0.0f)
	    v.bitangent = glm::normalize(v.bitangent);
    }
}