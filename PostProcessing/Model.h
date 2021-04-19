#pragma once

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Texture.h"


class Model : public Entity
{
public:
	Model();
	~Model();

	void Load(const char* filepath);
	void Create();
	void Render();

	void ResetModel();
	glm::mat4 GetModelMatrix();

private:

	void LoadNode(aiNode* node, const aiScene* scene);

	void LoadMesh(aiMesh* mesh, const aiScene* scene);

	std::string GetFolderName(const aiScene* scene, const char* filepath);

	std::vector<Mesh*> m_meshes;
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
};