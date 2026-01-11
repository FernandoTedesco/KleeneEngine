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