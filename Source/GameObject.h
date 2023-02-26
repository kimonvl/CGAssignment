#pragma once

#include "GeometryNode.h"
#include <glm/gtc/matrix_transform.hpp>
class GameObject {
public:

	GameObject() = default;
	GameObject(GeometryNode* nptr)
		: node(nptr) {}
	virtual ~GameObject() = default;
	
	virtual void Init(const std::string& name, GeometricMesh* mesh) { node = new GeometryNode(); node->Init(name, mesh); }

	virtual void Update(float dt);
	

	GeometryNode* GetGeometry() { return node; }
	const GeometryNode* GetGeometry() const { return node; }

	glm::mat4& GetTransform() { return app_model_matrix; }
	const glm::mat4& GetTransform() const { return app_model_matrix; }

	glm::mat4 model_matrix = glm::mat4(1.0f);
protected:

	glm::mat4 app_model_matrix = glm::mat4(1.0f);

private:

	GeometryNode* node = nullptr;

};