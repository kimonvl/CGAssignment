#pragma once
#include "MovementBoundaries.h"
#include "GameObject.h"

class Warrior : public GameObject
{
public:
	Warrior() = default;
	Warrior(GeometryNode* node, MovementBoundaries* mb )
		:GameObject(node), m_mb(mb){}
	void Update(float dt) override;
	void UpdateTransforms();
	inline const glm::vec3 GetPosition() const { return pos; }
	inline const glm::vec3 GetPosToBe() const { return posToBe; }
private:
	MovementBoundaries* m_mb = nullptr;
	glm::vec3 pos = {0.0f, 0.0f, 0.0f };
	glm::vec3 posToBe = { 0.0f, 0.0f, 0.0f };
	glm::vec3 angles = { 0.0f, 0.0f, 0.0f };
	glm::vec3 front = { 0.0f, 0.0f, -1.0f };
	float speed = 2.0f;
	float angularSpeed = 1.0f;
};
