#pragma once
#include "GameObject.h"
#include "Warrior.h"
#include "MovementBoundaries.h"

class Dragon : public GameObject
{
public:
	Dragon() = default;
	Dragon(GeometryNode * node, Warrior* war, glm::vec3 pos, MovementBoundaries* mb)
		:GameObject(node), warrior(war), position(pos), m_mb(mb)
	{
		m_mb->SetBlockState(pos.x, -pos.z, 6);
	}
	void Update(float dt) override;
private:
	void UpdateTransform();
private:
	float pickupDistance = 0.6f;
	glm::vec3 position;
	bool isActive = true;
	Warrior* warrior;
	MovementBoundaries* m_mb;
};
