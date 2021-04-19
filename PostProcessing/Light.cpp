#include "Light.h"

Light::Light()
{
	m_colour = glm::vec3(1.f);
	m_intensity = 1.f;
}

Light::Light(float shadowWidth, float shadowHeight, glm::vec3 colour, float intensity)
{
	m_shadowMap = new ShadowMap();
	m_shadowMap->Init(shadowWidth, shadowHeight);

	m_colour = colour;
	m_intensity = intensity;
}

Light::~Light()
{
}

void Light::SetColour(glm::vec3 colour)
{
	m_colour = colour;
}

void Light::SetIntensity(float intensity)
{
	m_intensity = intensity;
}

glm::vec3* Light::GetColourPtr()
{
	return &m_colour;
}

float* Light::GetIntensityPtr()
{
	return &m_intensity;
}

glm::vec3 Light::GetColour()
{
	return m_colour;
}

float Light::GetIntensity()
{
	return m_intensity;
}
