#pragma once

#include "PointLight.h"

class SpotLight : public PointLight
{
public:

	SpotLight();
	SpotLight(glm::vec3 position, glm::vec4 colour, glm::vec3 direction,
		float innerCutOff, float outterCutOff,
		unsigned int shadowWidth, unsigned int shadowHeight,
		float nearPlane, float farPlane);
	~SpotLight();

	void Bind(Shader* shader, int Index);

	inline void SetDirection(glm::vec3 direction) { m_direction = direction; };
	inline glm::vec3 GetDirection() { return m_direction; };
	inline glm::vec3* GetDirectionPtr() { return &m_direction; };

	inline void SetOutterCutOff(float outterCutOff) { m_outterCutOff = outterCutOff; };
	inline float GetOutterCutOff() { return m_outterCutOff; };
	inline float* GetOutterCutOffPtr() { return &m_outterCutOff; };

	inline void SetInnerCutOff(float innerCutOff) { m_innerCutOff = innerCutOff; };
	inline float GetInnerCutOff() { return m_innerCutOff; };
	inline float* GetInnerCutOffPtr() { return &m_innerCutOff; };

private:

	glm::vec3 m_direction;
	float m_innerCutOff;
	float m_outterCutOff;
};

