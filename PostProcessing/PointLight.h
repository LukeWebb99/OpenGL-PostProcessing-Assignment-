#pragma once

#include <vector>

#include <GL/glew.h>

#include "Light.h"
#include "Shader.h"
#include "OmniShadowMap.h"

class PointLight : public Light
{
public:
    
    PointLight();
    PointLight(glm::vec4 colour, glm::vec3 position, 
        float shadowWidth, float shadowHeight, 
        float nearPlane, float farPlane);
    ~PointLight();

    void Bind(Shader* shader, int index);

    std::vector<glm::mat4> CalculateLightTransform();
    inline float GetFarPlane() { return m_farPlane; };

    void SetPosition(glm::vec3 position);
    void SetConstant(float constant);
    void SetLinear(float linear);
    void SetQuadratic(float quadratic);

    glm::vec3 GetPosition();
    glm::vec3* GetPositionPtr();
    float GetConstant();
    float GetLinear();
    float GetQuadratic();

    float* GetConstantPtr();
    float* GetLinearPtr();
    float* GetQuadraticPtr();

private:
    glm::vec3 m_position;
    float m_constant;
    float m_linear;
    float m_exponent;
    float m_farPlane;
};

