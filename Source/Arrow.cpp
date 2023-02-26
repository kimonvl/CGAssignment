#include "Arrow.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

constexpr float halfpi = glm::pi<float>() / 2;

Arrow::Arrow(GeometryNode* node, const glm::mat4& transform, float ang ,MovementBoundaries* mb)
	:GameObject(node), m_mb(mb)
{
	app_model_matrix = transform;
	pos = glm::vec3(app_model_matrix[3]);
	angle = glm::radians(ang);
	direction = glm::vec3(glm::cos(angle + halfpi), 0.0f, glm::sin(angle + halfpi));
}

void Arrow::Update(float dt)
{
	
	pos += direction * dt * speed;
	app_model_matrix = glm::translate(glm::mat4(1.0f), pos) * 
		glm::translate(glm::mat4(1.f), GetGeometry()->m_aabb.center)*
		glm::rotate(glm::mat4(1.f), angle, glm::vec3(0.f, 1.f, 0.f))*
		glm::translate(glm::mat4(1.f), -GetGeometry()->m_aabb.center)* model_matrix;
	if (m_mb->BlockState(pos.x, -pos.z) != 1) {
		m_mb->SetBlockState(pos.x, -pos.z, 4);
		m_mb->SetBlockStateByIndex(previousIndex, 2);
		previousIndex = m_mb->FindIndex(pos.x, -pos.z);
	}
	/*
	accumulator += dt;
	
	pos = glm::lerp(initialPos, target, accumulator / travelTime);
	if (accumulator >= travelTime)
	{
		accumulator -= travelTime;
		pos = initialPos;
	}

	app_model_matrix = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	*/
}

void Arrow::Reset(float ang, const glm::vec3& position)
{
	angle = ang;
	pos = position;
	direction = glm::vec3(glm::sin(angle), 0.0f, glm::cos(angle));
}

