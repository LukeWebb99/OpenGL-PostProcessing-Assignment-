#include "Texture.h"

//view and projection matrix for prospective
const glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
const glm::mat4 captureViews[] = { // matrices used for projecting cubemap
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};

Texture::Texture() {

	m_id = NULL;
	m_width = NULL;
	m_height = NULL;
	m_textureType = NULL;
	m_captureFBO = NULL;
	m_captureRBO = NULL;
	m_components = NULL;
	m_path = nullptr;
	m_image2D = nullptr;
	m_imageHDRI = nullptr;
}

Texture::Texture(const char* path) {

	m_id = NULL;
	m_width = NULL;
	m_height = NULL;
	m_textureType = GL_TEXTURE_2D;
	m_captureFBO = NULL;
	m_captureRBO = NULL;
	m_components = NULL;
	m_path = path;
	m_image2D = nullptr;
	m_imageHDRI = nullptr;
}

Texture::~Texture() {
	glDeleteTextures(1, &m_id); // delete texture
}

GLuint const Texture::GetTextureID()
{
	return m_id;
}

void Texture::Bind(const GLint textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit); // bind texture
	glBindTexture(this->m_textureType, m_id);
}

void Texture::Unbind()
{
	glActiveTexture(0); // unbind texture
	glBindTexture(m_textureType, 0);
}

void Texture::LoadImageData()
{
	m_image2D = stbi_load(m_path, &m_width, &m_height, &m_components, 0); // load data from file using stb
}

void Texture::LoadHDRIData()
{
	stbi_set_flip_vertically_on_load(true); //flip file data
	m_imageHDRI = stbi_loadf(m_path, &this->m_width, &this->m_height, &this->m_components, NULL); // load data
}

void Texture::CreateDrawTexture(unsigned int width, unsigned int height)
{
	this->m_textureType = GL_TEXTURE_2D; // set texture type

	glGenTextures(1, &m_id); // gen texture
	glBindTexture(GL_TEXTURE_2D, m_id); // bind texture

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);	// Framebuffer texture size - resolution scale, i.e., width * height = 100% scale

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // set texture properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0); // bind texture 0
}

void Texture::CreateTexture2D()
{
	LoadImageData();//load data
	if (!m_image2D) { // if no data 

		if(!m_path) // if no path
			std::cout << "[ERROR]: No image path to load \n"; // print error
		else
			std::cout << "[ERROR]: Failed to load texture " << '"' << m_path << '"' << "\n"; // if path has data but no image data print error 
	}
	else {
		glGenTextures(1, &m_id); // gen texture
	    glBindTexture(GL_TEXTURE_2D, m_id); // bind

	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set properties
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR); // set mipmap options
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	    
		switch (m_components)
		{

		case 4: // 4 components use RGBA
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image2D);

		case 3: // 2 components use RGB
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_image2D);

		default: 
			break;
		}
	

	    glGenerateMipmap(GL_TEXTURE_2D); // gen mipmap texture
		glBindTexture(GL_TEXTURE_2D, NULL); // bind texture 0
		
		stbi_image_free(m_image2D); // delete file data, no longer needed as its been passed to gpu
	}

	
}

void Texture::LoadCubemap(const char* rightFace, const char* leftFace, 
     const char* topFace, const char* bottomFace, 
	 const char* backFace, const char* frontFace) {

	std::string imagePaths[6]; // get path for each face
	imagePaths[0] = rightFace;
	imagePaths[1] = leftFace;
	imagePaths[2] = topFace;
	imagePaths[3] = bottomFace;
	imagePaths[4] = backFace;
	imagePaths[5] = frontFace;

	m_textureType = GL_TEXTURE_CUBE_MAP; // set texture type

	glGenTextures(1, &m_id); // gen texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id); // bind texture

	for (size_t i = 0; i < 6; i++) { // for each face of cube

		unsigned char* image = stbi_load(imagePaths[i].c_str(), &m_width, &m_height, &m_components, 0); // load texture data

		if (!image) { // if no data 
			std::cout << "[ERROR]: Failed to load cubemap " << '"' << imagePaths[i].c_str() << '"' << "\n"; // print error
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image); // set data for face
		stbi_image_free(image); // delete data

	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // set texture properties 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, NULL); // bind texture 0

}

void Texture::CreateHDRI() {

	m_textureType = GL_TEXTURE_2D; // set texture type
	
	this->LoadHDRIData(); // load hdri data
	if (!m_imageHDRI) { // no data
		
		if (!m_path) // if no path data
			std::cout << "[ERROR]: No HDRI image path to load \n"; // print error saying no path data
		else
			std::cout << "[ERROR]: Failed to load texture " << '"' << m_path << '"' << "\n"; // print error saying no image data
	}
	else {

		glGenTextures(1, &m_id); // gen texture
		glBindTexture(GL_TEXTURE_2D, m_id); // bind texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, m_imageHDRI); // pass data to gpu

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set properties
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


		glBindTexture(GL_TEXTURE_2D, NULL); // bind texture 0

	}
	stbi_image_free(m_imageHDRI); // delete data

}

void Texture::CreateCubemapFromHDRI(Texture HDRI) {

	this->m_textureType = GL_TEXTURE_CUBE_MAP; // set texture type

	//Create Shader for converting;
	Shader equirectangularToCubemapShader; // create shader for converting retangle image to cubemap
	equirectangularToCubemapShader.CreateFromFile("Shaders/EquirectangularToCubemapVert.glsl", "Shaders/EquirectangularToCubemapFrag.glsl"); // load shader data
	
	//create framebuffer and renderbuffer
	glGenFramebuffers(1, &m_captureFBO); 
	glGenRenderbuffers(1, &m_captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO); // bind frame and render buffers
	glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512); //	 alloc renderbuffer storage 
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_captureRBO); // attach renderbuffer to framebuffer

	//create empty cubemap to place results into
	glGenTextures(1, &this->m_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->m_id);
	for (unsigned int i = 0; i < 6; i++)
	{
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// convert HDR equirectangular environment map to cubemap equivalent
	HDRI.Bind(0);
	equirectangularToCubemapShader.Bind();
	equirectangularToCubemapShader.Set1i(0, "u_equirectangularMap");
	equirectangularToCubemapShader.SetMat4f(captureProjection, "u_projection", false);

	glViewport(0, 0, 512, 512); // set view port to the same size as cubemap face size
	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO); // bind framebuffer
	for (unsigned int i = 0; i < 6; i++) // for each face
	{
		equirectangularToCubemapShader.SetMat4f(captureViews[i], "u_view", false); // use view matrix for that face
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->m_id, 0); // convert framebuffer to texture
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear

		renderCube(); // render cube
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind framebuffer 0

	glDeleteBuffers(1, &m_captureFBO); // delete both buffers
	glDeleteRenderbuffers(1, &m_captureRBO);
}

void Texture::CreateIrradianceTexture(Texture* Cubemap) {

	this->m_textureType = GL_TEXTURE_CUBE_MAP; // set texture type
	
	Shader IrradianceConvolutionShader; // init Irradiance Convolution Shader, used for bluring or conver convoluting texture
	IrradianceConvolutionShader.CreateFromFile("Shaders/SkyboxVert.glsl", "Shaders/IrradianceConvolutionFrag.glsl"); // load file
	
	glGenTextures(1, &m_id); // gen texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id); // bind texture

	for (size_t i = 0; i < 6; i++) // for each face
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr); // create texture with no data
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // set properties for texture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &m_captureFBO); // gen frame and render buffers
	glGenRenderbuffers(1, &m_captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);  // bind frame and render buffers
	glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32); // alloc renderbuffer size

	Cubemap->Bind(0); // bind cubemap texture for convoluting
	IrradianceConvolutionShader.Bind(); // bind shader 
	IrradianceConvolutionShader.Set1i(0, "u_environmentMap"); // bind texture
	IrradianceConvolutionShader.SetMat4f(captureProjection, "u_projectionMatrix", false); // set projection matrices 

	glViewport(0, 0, 32, 32); // set view port to size of buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO); // bind framebuffer
	for (unsigned int i = 0; i < 6; i++) // for each face
	{
		IrradianceConvolutionShader.SetMat4f(captureViews[i], "u_viewMatrix", false); // set view matrix of matching face
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_id, 0); //store framebuffer image in texture
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear buffer

		renderCube(); // draw cube
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind frambuffer 0
	glDeleteBuffers(1, &m_captureFBO); // delete buffers
	glDeleteRenderbuffers(1, &m_captureRBO);
}

void Texture::CreatePrefilterMap(Texture* Cubemap) {

	this->m_textureType = GL_TEXTURE_CUBE_MAP; // set texture type

	Shader prefilterShader; // init prefilter shader
	prefilterShader.CreateFromFile("Shaders/SkyboxVert.glsl", "Shaders/PrefilterFrag.glsl"); // load shader

	glBindTexture(GL_TEXTURE_CUBE_MAP, Cubemap->GetTextureID()); // bind texture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // set properties
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP); // gen mipmap
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // bind texture 0

	glGenTextures(1, &m_id); // gen texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id); // bind textures

	for (unsigned int i = 0; i < 6; i++) //for each face
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr); // alloc texture with no data
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // set texture data
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP); // gen mipmaps

	prefilterShader.Bind(); // bind prefilter shader 
	prefilterShader.Set1i(0, "u_environmentMap"); // set location of texture
	prefilterShader.SetMat4f(captureProjection, "u_projectionMatrix", false); // set projection matrix
	Cubemap->Bind(0); // bind cubemap texture

	glGenFramebuffers(1, &m_captureFBO); //gen frame and render buffers
	glGenRenderbuffers(1, &m_captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO); // bind framebuffer

	unsigned int maxMipLevels = 5; // how many mipmap levels to filter
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip) // for each filter level
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 512 * std::pow(0.5, mip); // scale by mipmap size
		unsigned int mipHeight = 512 * std::pow(0.5, mip);

		glBindRenderbuffer(GL_RENDERBUFFER, m_captureFBO); //bind renderbuffer
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight); // attach renderbuffer to framebuffer
		glViewport(0, 0, mipWidth, mipHeight); // set viewport to size of mipmap level

		float roughness = (float)mip / (float)(maxMipLevels - 1); //calculate roughness
		prefilterShader.Set1f(roughness, "u_roughness"); // set roughness
		for (unsigned int i = 0; i < 6; i++) // for each face
		{
			prefilterShader.SetMat4f(captureViews[i], "u_viewMatrix", false); // set view matrix
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_id, mip); // attach framebuffer to texture

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear
			renderCube(); // render cube
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind framebuffer 0

	glDeleteBuffers(1, &m_captureFBO); // delete framebuffer
	glDeleteRenderbuffers(1, &m_captureRBO); // render renderbuffer

}

void Texture::CreateBRDFLookUpTable() {

	this->m_textureType = GL_TEXTURE_2D; // set texture type

	Shader brdfShader; // init brdf shader
	brdfShader.CreateFromFile("Shaders/BRDFShaderVert.glsl", "Shaders/BRDFShaderFrag.glsl"); // load shaders

	glGenTextures(1, &m_id); // gen textures
	glBindTexture(GL_TEXTURE_2D, m_id); // bind textures

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0); // alloc size of texture

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // set texture properties 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &m_captureFBO); // gen framebuffers
	glGenRenderbuffers(1, &m_captureRBO); // gen renderbuffers

	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO); //bind buffers
	glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512); // init data storage 
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_id, 0); // attach framebuffer to texture
	
	glViewport(0, 0, 512, 512); // set view port to size of buffers 
	brdfShader.Bind(); // bind texture
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear
	renderQuad(); // draw quad

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind framebuffer 0 

	glDeleteFramebuffers(1, &m_captureFBO); // delete buffers
	glDeleteRenderbuffers(1, &m_captureRBO);
	
}
