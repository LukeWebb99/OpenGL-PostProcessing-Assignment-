#include "SpotLight.h"

SpotLight::SpotLight(glm::vec3 position, glm::vec4 colour, glm::vec3 direction, float innerCutOff, float outterCutOff,
	unsigned int shadowWidth, unsigned int shadowHeight, float nearPlane, float farPlane)
	: PointLight(colour, position, shadowWidth, shadowHeight, nearPlane, farPlane)
{
	m_direction = glm::normalize(direction);
	m_innerCutOff = glm::radians(innerCutOff);
	m_outterCutOff = glm::radians(outterCutOff);
}

SpotLight::~SpotLight()
{
}

void SpotLight::Bind(Shader* shader, int Index) {

	//set uniforms
	shader->SetVec3f(GetPosition(), "spotlight.base.m_position");
	shader->SetVec3f(GetDirection(), "spotlight.m_direction");
	shader->SetVec4f(glm::vec4(GetColour(), GetIntensity()), "spotlight.base.m_colour");

	shader->Set1f(GetConstant(), "spotlight.base.m_constant");
	shader->Set1f(GetLinear(), "spotlight.base.m_linear");
	shader->Set1f(GetQuadratic(), "spotlight.base.m_quadratic");

	shader->Set1f(GetOutterCutOff(), "spotlight.m_outterEdge");
	shader->Set1f(GetInnerCutOff(), "spotlight.m_innerEdge");

	GetShadowMapPtr()->BindTexture(TU_SPOT_SHADOW_MAP0);
	shader->Set1i(TU_SPOT_SHADOW_MAP0, "spotlight.base.m_omniShadowMap");
	shader->Set1f(GetFarPlane(), "spotlight.base.m_omniFarPlane");

}


