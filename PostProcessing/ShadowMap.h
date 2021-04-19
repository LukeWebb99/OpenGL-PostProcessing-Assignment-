#pragma once

#include <gl/glew.h>
#include <stdio.h>

class ShadowMap
{
public:
	ShadowMap();
	~ShadowMap();

	virtual bool Init(unsigned int width, unsigned int height);

	virtual void BindFBO();

	virtual void BindTexture(const int textureUnit);

	unsigned int inline GetWidth() { return m_width; };
	unsigned int inline GetHeight() { return m_height; };

protected:

	unsigned int m_FBO, m_shadowMap;
	unsigned int m_width, m_height;
};

