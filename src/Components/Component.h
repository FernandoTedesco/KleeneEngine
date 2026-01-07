#pragma once
class GameObject;

class Component
{
public:
    GameObject* owner;
    bool enabled = true;
    virtual ~Component()
    {
    }
    virtual void Start()
    {
    }
    virtual void Update(float dt)
    {
    }
    virtual void Render()
    {
    }
};