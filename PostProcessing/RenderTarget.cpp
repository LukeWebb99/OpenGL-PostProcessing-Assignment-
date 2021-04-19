#include "RenderTarget.h"

RenderTarget::RenderTarget(unsigned int width, unsigned int height) {
	
	m_BufferWidth = width; m_BufferHeight = height; // set buffer size

	// Generate and bind a framebuffer
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	// Generate and bind textures
	m_Buffer = Texture(); // buffer will be a texture
	m_Buffer.CreateDrawTexture(width, height); // call function to create texture with no data
	m_Buffer.Bind(0); // bind to unit 0

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Buffer.GetTextureID(), 0); //bind texture to framebuffer

	glGenRenderbuffers(1, &m_RBO); //gen render buffer
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO); // bind render buffer

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // alloc data for buffer

	glBindRenderbuffer(GL_RENDERBUFFER, 0); // bind render buffer 0

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO); // attach render buffer to framebuffer

	// Check framebuffer is correctly configured
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "RenderTarget framebuffer failed..\n"; 
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //bind framebuffer 0
	m_Buffer.Unbind(); //unbind texture
}

RenderTarget::~RenderTarget()
{
	//clean up and delete buffers
	if(m_FBO)
		glDeleteFramebuffers(1, &m_FBO);

	if (m_RBO)
		glDeleteRenderbuffers(1, &m_RBO);
}

void RenderTarget::Bind(Window& window) {
	
	//bind framebuffer
	window.SetViewport(); //set view port same size as window
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); //Bind Framebuffer
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO); //Bind Renderbuffer
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window.GetBufferWidth(), window.GetBufferHeight()); //resize framebuffer to window size
	
	m_Buffer.Bind(0); //bind buffer texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.GetBufferWidth(), window.GetBufferHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); //resize texture

	glEnable(GL_DEPTH_TEST); // enable depth test

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f); //clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void RenderTarget::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind default framebuffer
	glDisable(GL_DEPTH_TEST); // disable depth testing
}

Texture* RenderTarget::GetTexture()
{
	return &m_Buffer; // returns buffer texture
}
