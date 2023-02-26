#include "Totem.h"

static float continous_time = 0.0f;


void Totem::Update(float dt)
{
	continous_time += dt;
	accumulator += dt;
	if (accumulator >= cooldown)
	{
		accumulator -= cooldown;
		m_arrow1->Reset(continous_time, app_model_matrix[3]);
		m_arrow2->Reset(continous_time + glm::pi<float>(), app_model_matrix[3]);
	}

	
	UpdateTransforms();
}




void Totem::UpdateTransforms()
{
	//m_arrow->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f));
	app_model_matrix = glm::translate(glm::mat4(1.f), position) *
		glm::translate(glm::mat4(1.f), GetGeometry()->m_aabb.center) *
		glm::rotate(glm::mat4(1.f), continous_time, glm::vec3(0.f, 1.f, 0.f)) *
		glm::translate(glm::mat4(1.f), -GetGeometry()->m_aabb.center) * model_matrix;
}
