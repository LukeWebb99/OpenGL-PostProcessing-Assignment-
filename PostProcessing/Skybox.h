#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class Skybox
{
public:

	Skybox();
	Skybox(Texture* cubemap);

   ~Skybox();

   void Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
   void SetCubemap(Texture* cubemap);

private:

	void CreateMesh();

	Mesh* m_mesh;
	Shader* m_shader;
	Texture* m_cubemap;
	GLuint m_cubemapID;
	unsigned int envCubemap;
};

