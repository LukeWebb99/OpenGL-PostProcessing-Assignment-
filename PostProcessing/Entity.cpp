#include "Entity.h"

/*
	entity class used to store to control model matrix information for drawable objects
*/

Entity::Entity()
{
	m_model = glm::mat4(1.f); //set model to defalut
	m_translate = glm::vec3(0.f, 0.f, 0.f);
	m_rotation  = glm::vec3(0.f, 0.f, 0.f);
	m_scale     = glm::vec3(1.f, 1.f, 1.f);
}

Entity::~Entity() {}

void Entity::SetTranslation(glm::vec3 translate)
{
	m_translate = translate;
}

void Entity::SetRotation(glm::vec3 rotation)
{
	m_rotation = rotation;
}

void Entity::SetScale(glm::vec3 scale)
{
	m_scale = scale;
}

glm::vec3 Entity::GetTranslation()
{
	return m_translate;
}

glm::vec3 Entity::GetRotation()
{
	return m_rotation;
}

glm::vec3 Entity::GetScale()
{
	return m_scale;
}

void Entity::SetModel(glm::mat4* model)
{
	m_model = *model;
}

glm::mat4 Entity::GetModel()
{
	this->UpdateModel();
	return m_model;
}

glm::mat4* Entity::GetModelPtr()
{
	return &m_model;
}

void Entity::UpdateModel()
{
	m_model = glm::mat4(1.f); // set to identity 
	m_model = glm::translate(m_model, m_translate); //translate
	
	m_model = glm::rotate(m_model, glm::radians(m_rotation.x), glm::vec3(1.f, 0.f, 0.f)); //rotate
	m_model = glm::rotate(m_model, glm::radians(m_rotation.y), glm::vec3(0.f, 1.f, 0.f));
	m_model = glm::rotate(m_model, glm::radians(m_rotation.z), glm::vec3(0.f, 0.f, 1.f));

	m_model = glm::scale(m_model, m_scale); // scale
}

void Entity::ResetModel()
{
	m_model = glm::mat4(1.f); // reset model to identity
	m_translate = glm::vec3(0.f, 0.f, 0.f); // set to 0
	m_rotation = glm::vec3(0.f, 0.f, 0.f); // set to 0
	m_scale = glm::vec3(1.f, 1.f, 1.f); // set to 1
}
