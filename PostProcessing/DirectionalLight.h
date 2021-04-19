#pragma once

#include "Light.h"
#include "Shader.h"
#include "Material.h"

class DirectionalLight :
	public Light
{
public:
	DirectionalLight();
	DirectionalLight(float shadowWidth, float shadowHeight, int filterLevel, glm::vec3 colour, float intensity, glm::vec3 direction);
	~DirectionalLight();

	void Bind(Shader* shader);

	glm::vec3  GetDirection();
	glm::vec3 *GetDirectionPtr();
	glm::mat4 CalculateLightTransform();
	int GetFilterLevel();
	int *GetFilterLevelPtr();

private:
	glm::vec3 m_direction;
	int m_filterLevel;
};

