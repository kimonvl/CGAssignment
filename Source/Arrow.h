#pragma once

#include "GameObject.h"
#include "MovementBoundaries.h"

class Arrow : public GameObject
{
public:
	Arrow() = default;
	Arrow(GeometryNode* node, const glm::mat4& transform, float ang, MovementBoundaries* mb);
	void Update(float dt) override;
	void Reset(float ang, const glm::vec3& position);
private:
	glm::vec3 pos;
	glm::vec3 direction;
	int previousIndex = 0;
	MovementBoundaries* m_mb;
	std::vector<glm::vec2> squaresToReset;
	float speed = 1.0f;
	float accumulator = 0.0f;
	float travelTime = 5.0f;
	float angle = 0.0f;
};
