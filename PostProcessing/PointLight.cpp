#include "PointLight.h"

PointLight::PointLight()
{
    m_position = glm::vec3(0.f);
    m_constant = 1.f;
    m_linear = 1.f;
    m_exponent = 1.f;
    m_farPlane = 50;
}

PointLight::PointLight(glm::vec4 colour, glm::vec3 position, float shadowWidth, float shadowHeight, float nearPlane, float farPlane)
    :Light(shadowWidth, shadowHeight, glm::vec3(colour.x, colour.y, colour.z), colour.w)
{
    m_position = position;
    m_farPlane = farPlane;

    m_constant = 1.f;
    m_linear = 1.f;
    m_exponent = 1.f;

    float aspectRatio = ((float)shadowWidth / (float)shadowHeight);
    m_projection = glm::perspective(glm::radians(90.f), aspectRatio, nearPlane, farPlane);

    m_shadowMap = new OmniShadowMap();
    m_shadowMap->Init(shadowWidth, shadowHeight);

}

PointLight::~PointLight()
{
}

void PointLight::Bind(Shader* shader, int index)
{
    // set uniforms
    std::string pos = "pointLight[" + std::to_string(index) + "].m_position";
    shader->SetVec3f(GetPosition(), pos.c_str());

    std::string col = "pointLight[" + std::to_string(index) + "].m_colour";
    shader->SetVec4f(glm::vec4(GetColour(), GetIntensity()), col.c_str());

    std::string shadowMap = "pointLight[" + std::to_string(index) + "].m_omniShadowMap";
    shader->Set1i(TU_POINT_SHADOW_MAP0 + index, shadowMap.c_str());

    std::string shadowFarPlane = "pointLight[" + std::to_string(index) + "].m_omniFarPlane";
    shader->Set1f(GetFarPlane(), shadowFarPlane.c_str());

    GetShadowMapPtr()->BindTexture(TU_POINT_SHADOW_MAP0 + index);

}

std::vector<glm::mat4> PointLight::CalculateLightTransform()
{
    std::vector<glm::mat4> lightTransforms;

    //pos x
    lightTransforms.push_back(m_projection * glm::lookAt(m_position, m_position + glm::vec3(1, 0, 0) , glm::vec3(0, -1, 0)));
    //neg x
    lightTransforms.push_back(m_projection * glm::lookAt(m_position, m_position + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)));
    //pos y
    lightTransforms.push_back(m_projection * glm::lookAt(m_position, m_position + glm::vec3(0, 1, 0) , glm::vec3(0, 0, 1)));
    //neg y
    lightTransforms.push_back(m_projection * glm::lookAt(m_position, m_position + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)));
    //pos z
    lightTransforms.push_back(m_projection * glm::lookAt(m_position, m_position + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)));
    //neg z
    lightTransforms.push_back(m_projection * glm::lookAt(m_position, m_position + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0)));

    return lightTransforms;
}

void PointLight::SetPosition(glm::vec3 position)
{
    m_position = position;
}

void PointLight::SetConstant(float constant)
{
    m_constant = constant;
}

void PointLight::SetLinear(float linear)
{
    m_linear = linear;
}

void PointLight::SetQuadratic(float exponent)
{
    m_exponent = exponent;
}

glm::vec3 PointLight::GetPosition()
{
    return m_position;
}

glm::vec3* PointLight::GetPositionPtr()
{
    return &m_position;
}

float PointLight::GetConstant()
{
    return m_constant;
}

float PointLight::GetLinear()
{
    return m_linear;
}

float PointLight::GetQuadratic()
{
    return m_exponent;
}

float* PointLight::GetConstantPtr()
{
    return &m_constant;
}

float* PointLight::GetLinearPtr()
{
    return &m_linear;
}

float* PointLight::GetQuadraticPtr()
{
    return &m_exponent;
}
