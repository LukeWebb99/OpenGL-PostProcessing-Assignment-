#pragma once

#include "Texture.h"

class Material
{
public:
	Material();
	Material(Texture& albedo, Texture& normal, Texture& roughness, Texture& ao, Texture& metallic, Texture& displacement);
    ~Material();

   void Bind();

private:
	Texture* m_albedo;
	Texture* m_normal;
	Texture* m_roughness;
	Texture* m_ao;
	Texture* m_metallic;
	Texture* m_displacement;
};

