#pragma once
#include <string>
#include <filesystem>

class ResourceManager{

    public:
    static std::string path;
    static std::filesystem::path FolderFinder(const std::string& foldername);
    
};
