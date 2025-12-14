#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include <string>
#include <filesystem>

class ResourceManager{

    public:
    static std::string path;
    static std::filesystem::path FolderFinder(const std::string& foldername);
    
};

#endif