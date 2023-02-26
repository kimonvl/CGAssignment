#pragma once

#include "GameObject.h"
#include "MovementBoundaries.h"
#include "Warrior.h"

class Door : public GameObject
{
public:
	Door(GeometryNode* node, Warrior* war, float ang, glm::vec3 position, glm::vec3 offset, MovementBoundaries* mb)
		:GameObject(node), m_war(war), angle(ang), pos(position), m_offset(offset), m_mb(mb)
	{

	}

	void Update(float dt) override;
	void UpdateTransform(glm::vec3 p);
	void Open();
	void Close();

private:

	enum class State : char
	{
		idle = 0, opening, closing
	};
	Warrior* m_war = nullptr;
	MovementBoundaries* m_mb;
	glm::vec3 pos;
	glm::vec3 m_offset;
	State state = State::idle;
	float duration = 1.5f;
	float accumulator = 0.0f;
	float angle;
	float additionalAngle = 0.0f;
};
