# Material

Version (0.5.X)

Files: Material.h / Material.cpp

## OverView

**Brief:** Material abstraction that binds visual parameters and textures into shaders

---

Currently the material class represents a simple render material to be used during draw calls. It acts as container for visual parameters (color tint, tiling), optional diffuse texture and contains the logic to bind these parameters to a shader.

It does not manage resource life-cycle(that would be the resourceManager job)

Important to mention this might be better as an struct in the future but since the material work is relatively new i decided to stick with a class based architecture for this module atleast for now.

## Full Documentation

Files: Material.h / Material.cpp

Role: Runtime material data container and shader binding helper

#### ResourceManager.h

1.1 Includes

Only glm.hpp necessary to use their data structures like vec2 and  vec3.

```cpp
#pragma once
#include <glm/glm.hpp>
```

1.2 Forwarding

Declaring classes through forwarding for them to be used in the shader binding mechanism.

```cpp
class Texture;
class Shader;
```

1.3 Components

regarding the class Material

#### Public

Standard construction method

```cpp
Material();
```

diffuse map is the main texture of a material

```cpp
Texture* diffuseMap = nullptr;
```

    colorTint is a sort of  color multiplier to a texture, allowing reusing the same texture with different tints.

Tiling is a parameter that controls how many times a texture repeats at a mesh's surface.

```cpp
glm::vec3 colorTint = glm::vec3(1.0f,1.0f,1.0f);
glm::vec2 tiling = glm::vec2(1.0f,1.0f);
```

Function to bind a material to a shader

```cpp
void Use(Shader* shader);
```



#### Material.cpp

Includes here are straightforward, texture, material and shaders all necessary for the binding logic

```cpp
#include  "Material.h"
#include "Shader.h"
#include "Texture.h"
```

Currently only for looks really, but it might be needed sometime soon.

```cpp
Material::Material()
{
}
```

Binds a material to a shader

```cpp
void Material::Use(Shader* shader)
{
    shader->SetVec3("material.color", colorTint);
    shader->SetVec2("material.tiling", tiling);
    if(diffuseMap != nullptr)
    diffuseMap->Use(0);
    shader->SetInt("material.diffuse", 0);

}
```






