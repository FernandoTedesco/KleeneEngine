#include  "Material.h"
#include "Shader.h"
#include "Texture.h"


Material::Material()
{
    static Texture* defaultTexture = new Texture();
    this->diffuseMap = defaultTexture;
}
void Material::Use(Shader* shader)
{
    shader->SetVec3("material.color", colorTint);
    shader->SetVec2("material.tiling", tiling);
    if(diffuseMap != nullptr)
    diffuseMap->Use(0);
    shader->SetInt("material.diffuse", 0);

    
   
}