#pragma once

#include "MovementBoundaries.h"
#include "GameObject.h"
#include "Arrow.h"


class Totem : public GameObject
{
public:
	Totem(GeometryNode* node, glm::vec3 pos, MovementBoundaries * mb, Arrow* arrow1, Arrow* arrow2)
		:GameObject(node), position(pos) ,m_mb(mb), m_arrow1(arrow1), m_arrow2(arrow2) {}

	void Update(float dt) override;
	void UpdateTransforms();
private:
	MovementBoundaries* m_mb;
	glm::vec3 position;
	Arrow* m_arrow1;
	Arrow* m_arrow2;
	float accumulator = 0.0f;
	float cooldown = 5.0f;
};