#include "Dragon.h"



void Dragon::Update(float dt)
{
	if ((glm::abs(position.x - warrior->GetPosition().x) <= pickupDistance) && (glm::abs(position.z - warrior->GetPosition().z) <= pickupDistance) && isActive) {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		m_mb->PickDragon();
		isActive = false;
		UpdateTransform();
	}

}

void Dragon::UpdateTransform()
{
	app_model_matrix = glm::translate(glm::mat4(1.0f), position) * model_matrix;
}
