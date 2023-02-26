#include "Door.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

int CalcSquareDistance(glm::ivec2 p0, glm::ivec2 p1)
{
	glm::ivec2 temp = p0 - p1;
	return temp.x * temp.x + temp.y * temp.y;
}

void Door::Update(float dt)
{
	if (CalcSquareDistance(m_mb->CoordsToIJ(pos.x, -pos.z), m_mb->CoordsToIJ(m_war->GetPosition().x, -m_war->GetPosition().z)) <= 9) {
		if (state == State::idle)
		{
			state = State::opening;
			accumulator = 0.0f;
		}
		accumulator += dt;
		Open();
		
	}
	else {
		if (state == State::opening)
		{
			state = State::closing;
			accumulator = 0.0f;
		}
		accumulator += dt;
		Close();
	}
	UpdateTransform(pos);
}



void Door::UpdateTransform(glm::vec3 p)
{
	app_model_matrix = glm::translate(glm::mat4(1.0f), p) *
		//glm::translate(glm::mat4(1.f), GetGeometry()->m_aabb.center) *
		glm::rotate(glm::mat4(1.f), glm::radians(angle + additionalAngle), glm::vec3(0.f, 1.f, 0.f)); //*
		//glm::translate(glm::mat4(1.f), -GetGeometry()->m_aabb.center) * model_matrix;
}

void Door::Open()
{
	const float a = accumulator / duration;
	additionalAngle = glm::lerp(0.0f, 90.0f, a);
	additionalAngle = glm::min(additionalAngle, 90.0f);
}

void Door::Close()
{
	const float a = accumulator / duration;
	additionalAngle = glm::lerp(90.0f, 0.0f, a);
	additionalAngle = glm::max(additionalAngle, 0.0f);
	if (additionalAngle == 0.0f)
	{
		state = State::idle;
	}
}
