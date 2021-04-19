#pragma once

#include "Texture.h"
#include "Shader.h"
#include "Window.h"

class RenderTarget
{
public:

	RenderTarget(unsigned int width, unsigned int height);
	~RenderTarget();

	void Bind(Window &window);
	void Unbind();

	Texture* GetTexture();

private:
	GLint m_BufferWidth, m_BufferHeight;
	unsigned int m_FBO, m_RBO;
	Texture m_Buffer;

};

