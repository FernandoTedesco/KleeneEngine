#pragma once
#include <glm/glm.hpp>
#include <cmath>

class Math
{
public:
    static bool RayAABBIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 aabbMin,
				    glm::vec3 aabbMax, float& outDistance)
    {
	float tMin = 0.0f;
	float tMax = 100000.0f;
	glm::vec3 position = rayOrigin;
	glm::vec3 direction = rayDirection;
	if (glm::abs(direction.x) > 0.0001f)
	{
	    float t1 = (aabbMin.x - position.x) / direction.x;
	    float t2 = (aabbMax.x - position.x) / direction.x;
	    if (t1 > t2)
	    {
		float temp = t1;
		t1 = t2;
		t2 = temp;
	    }
	    if (t1 > tMin)
		tMin = t1;
	    if (t2 < tMax)
		tMax = t2;
	    if (tMin > tMax)
		return false;
	    if (tMax < 0)
		return false;
	} else
	{
	    if (position.x < aabbMin.x || position.x > aabbMax.x)
		return false;
	}
	if (glm::abs(direction.y) > 0.0001f)
	{
	    float t1 = (aabbMin.y - position.y) / direction.y;
	    float t2 = (aabbMax.y - position.y) / direction.y;
	    if (t1 > t2)
	    {
		float temp = t1;
		t1 = t2;
		t2 = temp;
	    }
	    if (t1 > tMin)
		tMin = t1;
	    if (t2 < tMax)
		tMax = t2;
	    if (tMin > tMax)
		return false;
	    if (tMax < 0)
		return false;
	} else
	{
	    if (position.y < aabbMin.y || position.y > aabbMax.y)
		return false;
	}
	if (glm::abs(direction.z) > 0.0001f)
	{
	    float t1 = (aabbMin.z - position.z) / direction.z;
	    float t2 = (aabbMax.z - position.z) / direction.z;
	    if (t1 > t2)
	    {
		float temp = t1;
		t1 = t2;
		t2 = temp;
	    }
	    if (t1 > tMin)
		tMin = t1;
	    if (t2 < tMax)
		tMax = t2;
	    if (tMin > tMax)
		return false;
	    if (tMax < 0)
		return false;
	} else
	{
	    if (position.z < aabbMin.z || position.z > aabbMax.z)
		return false;
	}
	outDistance = tMin;
	return true;
    }
};
