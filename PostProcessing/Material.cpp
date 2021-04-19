#include "Material.h"

Material::Material() {
	//ADD DEFALUT
}

Material::Material(Texture& albedo, Texture& normal, Texture& roughness, Texture& ao, Texture& metallic, Texture& displacement)
{
	m_albedo = &albedo;
	m_normal = &normal;
	m_roughness = &roughness;
	m_ao = &ao;
	m_metallic = &metallic;
	m_displacement = &displacement;
}

Material::~Material()
{
}

void Material::Bind()
{
	//bind textures to reserved texture units
	m_albedo->Bind(TU_ALBEDO);
	m_normal->Bind(TU_NORMAL);
	m_roughness->Bind(TU_ROUGHNESS);
	m_ao->Bind(TU_AO);
	m_metallic->Bind(TU_METALLIC);
	m_displacement->Bind(TU_DISPLACEMENT);
}
