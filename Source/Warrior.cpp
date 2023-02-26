#include "Warrior.h"
#include "Input.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

static float continous_time = 0.0f;

void Warrior::Update(float dt)
{
	continous_time += dt;
	const glm::vec3 offset = speed * front * dt;
	if (Input::IsKeyPressed(SDL_SCANCODE_T)) {
		posToBe += offset;
		if (m_mb->BlockState(posToBe.x, -posToBe.z) == 2 || m_mb->BlockState(posToBe.x, -posToBe.z) == 4)
			pos = posToBe;
	}
	if (Input::IsKeyPressed(SDL_SCANCODE_G)){
		posToBe -= offset;
		if (m_mb->BlockState(posToBe.x, -posToBe.z) == 2 || m_mb->BlockState(posToBe.x, -posToBe.z) == 4)
			pos = posToBe;
	}
	posToBe = pos;
	if(m_mb->BlockState(pos.x, -pos.z) == 4)
	{ 
		std::cout << "Died----------------------------------" << std::endl;
		pos = glm::vec3(0.0f);
		UpdateTransforms();
	}
	const glm::vec3 r = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	const glm::vec3 angle_offset = angularSpeed * r * dt;
	if (Input::IsKeyPressed(SDL_SCANCODE_H))
	{
		front = glm::normalize(front + angle_offset);
		angles.y = glm::atan(front.x, front.z) + glm::pi<float>();
	}
	if (Input::IsKeyPressed(SDL_SCANCODE_F))
	{
		front = glm::normalize(front - angle_offset);
		angles.y = glm::atan(front.x, front.z) + glm::pi<float>();
	}
	UpdateTransforms();
}

void Warrior::UpdateTransforms(void)
{
	app_model_matrix = glm::translate(glm::mat4(1.0f), pos) *
		glm::translate(glm::mat4(1.f), GetGeometry()->m_aabb.center) *
		glm::rotate(glm::mat4(1.0f), angles.y, {0.0f, 1.0f, 0.0}) *
		glm::translate(glm::mat4(1.f), -GetGeometry()->m_aabb.center) * model_matrix;
}
