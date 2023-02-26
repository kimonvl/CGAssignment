#pragma once

#include "GameObject.h"
#include "MovementBoundaries.h"

class Spike : public GameObject
{
public:
	Spike(GeometryNode* node, const glm::mat4& transform, float ang, MovementBoundaries* mb);
	void Update(float dt) override;
private:
	MovementBoundaries* m_mb = nullptr;
	float accumulator = 0.0f;
	float cooldown = 5.0f;
	float activeTime = 2.0f;
	glm::vec3 start;
	glm::vec3 target;
	glm::vec3 pos;
	float angle;
};