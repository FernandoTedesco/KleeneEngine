#include "Terrain.h"
#include "MeshRenderer.h"
#include "Graphics/Mesh.h"
#include "Scenes/GameObject.h"
#include "Resources/ResourceManager.h"

Terrain::Terrain()
{
}
Terrain::~Terrain()
{
}

void Terrain::SetResourceManager(ResourceManager* resourceManager)
{
    this->resourceManager = resourceManager;
}

void Terrain::Start()
{
    if (owner)
    {
	meshRenderer = owner->GetComponent<MeshRenderer>();
	if (!meshRenderer)
	{
	    meshRenderer = owner->AddComponent<MeshRenderer>();
	}
	uniqueMeshName = "ProceduralTerrain_" + std::to_string(owner->GetID());
    }
    if (width == 0)
	InitializeGrid(10, 10);
}

void Terrain::Update(float dt)
{
    if (isDirty)
    {
	RebuildMesh();
	isDirty = false;
    }
}

void Terrain::InitializeGrid(int w, int d, float size)
{
    this->width = w;
    this->depth = d;
    this->tileSize = size;
    this->heightMap.assign(width * depth, 0.0f);
    this->isDirty = true;
}

int Terrain::GetIndex(int x, int z) const
{
    if (x < 0 || x >= width || z < 0 || z >= depth)
	return -1;
    return z * width + x;
}

void Terrain::SetBlockHeight(int x, int z, float height)
{
    int index = GetIndex(x, z);
    if (index != -1 && heightMap[index] != height)
    {
	heightMap[index] = height;
	isDirty = true;
    }
}

void Terrain::RebuildMesh()
{
    if (!resourceManager || !meshRenderer)
	return;
    uint32_t meshID = resourceManager->CreateMesh(uniqueMeshName);

    Mesh* meshPtr = resourceManager->GetMesh(meshID);
    if (!meshPtr)
	return;

    meshPtr->vertices.clear();
    meshPtr->indexes.clear();

    int totalTiles = width * depth;
    meshPtr->vertices.reserve(totalTiles * 4);
    meshPtr->indexes.reserve(totalTiles * 6);

    int vertCounter = 0;
    glm::vec3 up(0, 1, 0);

    for (int z = 0; z < depth; z++)
    {
	for (int x = 0; x < width; x++)
	{
	    float y = heightMap[z * width + x];
	    float xPos = x * tileSize;
	    float zPos = z * tileSize;

	    Mesh::Vertex v[4];

	    v[0].position = glm::vec3(xPos, y, zPos);
	    v[0].normal = up;
	    v[0].textureCoordinates = glm::vec2(0, 0);

	    v[1].position = glm::vec3(xPos, y, zPos + tileSize);
	    v[1].normal = up;
	    v[1].textureCoordinates = glm::vec2(0, 1);

	    v[2].position = glm::vec3(xPos + tileSize, y, zPos + tileSize);
	    v[2].normal = up;
	    v[2].textureCoordinates = glm::vec2(1, 1);

	    v[3].position = glm::vec3(xPos + tileSize, y, zPos);
	    v[3].normal = up;
	    v[3].textureCoordinates = glm::vec2(1, 0);

	    for (int i = 0; i < 4; i++)
		meshPtr->vertices.push_back(v[i]);

	    meshPtr->indexes.push_back(vertCounter + 0);
	    meshPtr->indexes.push_back(vertCounter + 1);
	    meshPtr->indexes.push_back(vertCounter + 2);

	    meshPtr->indexes.push_back(vertCounter + 0);
	    meshPtr->indexes.push_back(vertCounter + 2);
	    meshPtr->indexes.push_back(vertCounter + 3);

	    vertCounter += 4;
	}
    }
    meshPtr->SetupMesh();
    meshRenderer->SetMesh(meshID);
}