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
    this->tileMap.assign(width * depth, 0);
    this->isDirty = true;
}

void Terrain::SetTileTexture(int x, int z, int textureIndex)
{
    int index = GetIndex(x, z);
    if (index != -1 && tileMap[index] != textureIndex)
    {
	tileMap[index] = textureIndex;
	isDirty = true;
    }
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
	    int tileIndex = tileMap[z * width + x];
	    float padding = 0.002f;
	    float uStep = 1.0f / (float)atlasCols;
	    float vStep = 1.0f / (float)atlasRows;
	    int atlasX = tileIndex % atlasCols;
	    int atlasY = tileIndex / atlasCols;
	    float u0 = (atlasX * uStep) + padding;
	    float v0 = (1.0f - ((atlasY + 1) * vStep)) + padding;
	    float u1 = (u0 + uStep) - padding;
	    float v1 = (1.0f - (atlasY * vStep)) - padding;

	    Mesh::Vertex v[4];

	    v[0].position = glm::vec3(xPos, y, zPos);
	    v[0].normal = up;
	    v[0].textureCoordinates = glm::vec2(u0, v0);

	    v[1].position = glm::vec3(xPos, y, zPos + tileSize);
	    v[1].normal = up;
	    v[1].textureCoordinates = glm::vec2(u0, v1);

	    v[2].position = glm::vec3(xPos + tileSize, y, zPos + tileSize);
	    v[2].normal = up;
	    v[2].textureCoordinates = glm::vec2(u1, v1);

	    v[3].position = glm::vec3(xPos + tileSize, y, zPos);
	    v[3].normal = up;
	    v[3].textureCoordinates = glm::vec2(u1, v0);

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
    float maxHeight = 0.0f;
    for (float h : heightMap)
    {
	if (h > maxHeight)
	    maxHeight = h;
    }
    meshPtr->boundsMin = glm::vec3(0.0f, 0.0f, 0.0f);
    meshPtr->boundsMax = glm::vec3(width * tileSize, maxHeight + 1.0f, depth * tileSize);
    meshPtr->SetupMesh();
    meshRenderer->SetMesh(meshID);
}