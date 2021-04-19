#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light()
{
	m_direction = glm::vec3(0.0f, -1.0f, 0.0f);
	m_projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
	m_filterLevel = 0;
}

DirectionalLight::DirectionalLight(float shadowWidth, float shadowHeight, int filterLevel, glm::vec3 colour, float intensity, glm::vec3 direction)
	: Light(shadowWidth, shadowHeight, colour,  intensity)
{
	m_direction = direction;
	m_projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f); // set light projection
	m_filterLevel = filterLevel;
}

glm::vec3 DirectionalLight::GetDirection()
{
	return m_direction;
}

glm::vec3* DirectionalLight::GetDirectionPtr()
{
	return &m_direction;
}

glm::mat4 DirectionalLight::CalculateLightTransform()
{
	// calculate transform matrix
	return m_projection * glm::lookAt(-m_direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));	
}

int DirectionalLight::GetFilterLevel()
{
	return m_filterLevel;
}

int* DirectionalLight::GetFilterLevelPtr()
{
	return &m_filterLevel;
}

DirectionalLight::~DirectionalLight() { }

void DirectionalLight::Bind(Shader* shader) {

	GetShadowMapPtr()->BindTexture(TU_DIRECTIONAL_SHADOW_MAP); // bind shadow map
	
	//set uniforms
	shader->SetVec4f(glm::vec4(m_colour, m_intensity), "DirLight.m_colour");
	shader->SetVec3f(m_direction, "DirLight.m_direction");
	shader->SetMat4f(CalculateLightTransform(), "u_DirectionLightTransform", false);
	shader->Set1i(TU_DIRECTIONAL_SHADOW_MAP, "DirLight.m_shadowMap");
	shader->Set1i(m_filterLevel, "DirLight.m_filterLevel");

}
