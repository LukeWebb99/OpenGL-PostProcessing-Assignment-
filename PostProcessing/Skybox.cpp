#include "Skybox.h"

Skybox::Skybox() {

	this->m_shader = new Shader(); //create shader 
	m_shader->CreateFromFile("Shaders/SkyboxVert.glsl", "Shaders/SkyboxFrag.glsl"); //load shader code

	this->m_mesh = nullptr;
	this->m_cubemapID = 0;

	m_cubemap = nullptr;
	CreateMesh(); // create cube mesh
}

Skybox::Skybox(Texture* cubemap)
{
	this->m_shader = new Shader(); //create shader 
	m_shader->CreateFromFile("Shaders/SkyboxVert.glsl", "Shaders/SkyboxFrag.glsl"); //load shader code

	this->m_mesh = nullptr;
	this->m_cubemapID = 0;

	m_cubemap = cubemap;
	CreateMesh(); // create cube mesh
}

Skybox::~Skybox()
{
}

void Skybox::Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	m_shader->Bind(); // bind shader

	m_shader->SetMat4f(viewMatrix, "u_viewMatrix", false); // set matrices 
	m_shader->SetMat4f(projectionMatrix, "u_projectionMatrix", false);

	m_cubemap->Bind(0); // bind cubemap

	m_mesh->Render(); // render mesh
}

void Skybox::SetCubemap(Texture* cubemap)
{
	this->m_cubemap = cubemap; // set cubemap texture
}

void Skybox::CreateMesh()
{
	this->m_mesh = new Mesh(); //init new mesh
	m_mesh->Create(GetCubeVerticesPtr(), GetCubeIndicesPtr(), cubeVerticesCount, cubeIndicesCount); //create mesh

}
