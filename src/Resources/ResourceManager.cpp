#include "ResourceManager.h"
#include <filesystem>


std::filesystem::path ResourceManager::FolderFinder(const std::string& foldername)
{
  std::filesystem::path currentPath;
  currentPath = std::filesystem::current_path();
  
  while(true)
  {
    
   if(std::filesystem::exists(currentPath/foldername))
   {

    return(currentPath);
   }
   else if (currentPath == currentPath.parent_path()){
    return std::filesystem::path();
   }
   
   else{
    currentPath = currentPath.parent_path();
   }
     
 }
}
