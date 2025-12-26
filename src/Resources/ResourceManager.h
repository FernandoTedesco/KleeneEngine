#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include <map>
class Mesh;
class Texture;
class Material;
class ResourceManager{

    public:
    ResourceManager();
    ~ResourceManager();
    uint32_t CreateMesh(const std::string& name ,std::filesystem::path filePath);
    Mesh* GetMesh(uint32_t id);

    Texture* GetTexture(uint32_t id);
    uint32_t CreateTexture(const std::string& name ,std::filesystem::path filePath);

    uint32_t CreateMaterial(const std::string& name, uint32_t textureID);
    Material* GetMaterial(uint32_t id);

    std::vector<std::string> materialNames;
    std::vector<std::string>meshNames;
    std::vector<std::string>textureNames;
    
    static std::filesystem::path FolderFinder(const std::string& foldername);
    private:
    std::map<std::string, uint32_t> meshMap;
    std::map<std::string, uint32_t> textureMap;
    std::map<std::string, uint32_t> materialMap;
    std::vector<Mesh*> meshVector;
    std::vector<Texture*> textureVector;
    std::vector<Material*> materialVector;

    
};
