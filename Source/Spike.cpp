#include "Spike.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

Spike::Spike(GeometryNode* node, const glm::mat4& transform, float ang, MovementBoundaries* mb)
	:GameObject(node) , angle(ang), m_mb(mb)
{
	app_model_matrix = transform;
	start = glm::vec3(app_model_matrix[3]);
	target = start + glm::vec3(0.0f, 1.5f, 0.0f);
}

void Spike::Update(float dt)
{
	accumulator += dt;
	if (accumulator >= cooldown)
	{
		accumulator -= cooldown;
		activeTime = 2.0f;
		/*
		glm::vec3 temp = start;
		start = target;
		target = temp;*/
		pos = target;
	}
	else {
		if (activeTime > 0) {
			activeTime -= dt;
			pos = target;
		}
		else {
			pos = start;
		}
		
	}
	if(pos.y >0.1)
		m_mb->ActivateTrap(start.x, -start.z, 4);
	else
		m_mb->ActivateTrap(start.x, -start.z, 2);

	app_model_matrix = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
}
