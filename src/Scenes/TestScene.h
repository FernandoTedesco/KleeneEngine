#include "Scene.h"
#include "Resources/ResourceManager.h"

class TestScene : public Scene
{
    public:
    TestScene(ResourceManager* resourceManager);
    void Init() override;
    void Update(float deltaTime) override;
    void Render() override;
};