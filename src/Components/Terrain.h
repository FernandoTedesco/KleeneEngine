#pragma once
#include "Components/Component.h"
#include <vector>
#include <string>
class Mesh;
class MeshRenderer;
class ResourceManager;

class Terrain : public Component
{
    friend class InspectorPanel;
    friend class SceneManager;
    friend class EditorTools;

public:
    Terrain();
    ~Terrain();

    std::vector<int> tileMap;
    int atlasCols = 16;
    int atlasRows = 16;
    std::string atlasTextureName = "atlas.png";
    void SetTileTexture(int x, int z, int textureIndex);

    void Start() override;
    void Update(float dt) override;

    void SetResourceManager(ResourceManager* resourceManager);
    void InitializeGrid(int width, int depth, float tileSize = 1.0f);
    void SetBlockHeight(int x, int z, float height);

private:
    int width = 0;
    int depth = 0;
    float tileSize = 1.0f;
    bool isDirty = false;

    std::string uniqueMeshName;

    std::vector<float> heightMap;

    MeshRenderer* meshRenderer = nullptr;
    ResourceManager* resourceManager = nullptr;

    void RebuildMesh();
    int GetIndex(int x, int z) const;
};