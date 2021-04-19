#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "ShadowMap.h"

class Light
{
public:

	Light();
	Light(float shadowWidth, float shadowHeight, glm::vec3 colour, float intensity);

	~Light();

	void SetColour(glm::vec3 colour);
	void SetIntensity(float intensity);

	glm::vec3 GetColour();
	float GetIntensity();
	
	glm::vec3* GetColourPtr();
	float    * GetIntensityPtr(); 

	ShadowMap* GetShadowMapPtr() { return m_shadowMap; };

protected:

	glm::vec3 m_colour;
	float     m_intensity;

	glm::mat4 m_projection; 
	ShadowMap* m_shadowMap;
};

