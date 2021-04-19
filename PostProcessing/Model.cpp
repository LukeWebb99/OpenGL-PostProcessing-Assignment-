#include "Model.h"

Model::Model()
{
}

Model::~Model()
{
}

void Model::Load(const char* filepath) {

	Assimp::Importer importer; // init importer
	// init scene and use importer to load from file
	const aiScene* scene = importer.ReadFile(filepath, 
		aiProcess_Triangulate | // triangulate 
		aiProcess_FlipUVs | // filp uvs
		aiProcess_CalcTangentSpace | // calculate tangent
		aiProcess_GenSmoothNormals |  // get smooth normals
		aiProcess_JoinIdenticalVertices); // join identical vertex data

	if (!scene) { // if scene is null print error 
		printf("[MODEL LOADER]: %s\n", importer.GetErrorString());
		return;
	}
	else { // else start loading data
		printf("[MODEL LOADER]: Opening - %s \n", filepath);
		LoadNode(scene->mRootNode, scene);
	}

}

void Model::Create() {

	Mesh* m = new Mesh(m_vertices, m_indices); // create mesh
	m_meshes.push_back(m); // push to array of meshes 

}

void Model::Render()
{
	UpdateModel();
	for (auto m : m_meshes) { // for every mesh
		m->SetModel(this->GetModelPtr()); //set model to model classes model matrix
		m->Render(); // render 
	}
}

void Model::ResetModel()
{
	for (auto m : m_meshes) // for every mesh reset model
		m->ResetModel();
}

glm::mat4 Model::GetModelMatrix()
{
	return this->GetModel();
}

void Model::LoadNode(aiNode* node, const aiScene* scene) {

	for (size_t i = 0; i < node->mNumMeshes; i++) // for all meshes
	{
		LoadMesh(scene->mMeshes[node->mMeshes[i]], scene); // load mesh
	}

	for (size_t i = 0; i < node->mNumChildren; i++) // for all child nodes in file
	{
		LoadNode(node->mChildren[i], scene); // load nodes
	}

}

void Model::LoadMesh(aiMesh* mesh, const aiScene* scene) {
	
	bool loadTangentAndBitangents;
	// check if has tangents and bitangents
	mesh->HasTangentsAndBitangents() ? loadTangentAndBitangents = true : loadTangentAndBitangents = false;

	m_vertices = std::vector<Vertex>(mesh->mNumVertices); // allow std::vector to reserve space for data ahead of time, should speed things up
	
	Vertex v; // temp vertex for storing data
	for (size_t i = 0; i < mesh->mNumVertices; i++) { // for every vertex in mesh
		
		v.m_position =  glm::vec3(mesh->mVertices[i].x ,  mesh->mVertices[i].y,   mesh->mVertices[i].z); // get position
		v.m_normal =    glm::vec3(mesh->mNormals[i].x,    mesh->mNormals[i].y,    mesh->mNormals[i].z); // get normal 
		if (loadTangentAndBitangents) { // if tangents
			v.m_tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z); // get tangents
			v.m_bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
		}
		else { // else init vectors at 0
			v.m_tangent   = glm::vec3(0.f);
			v.m_bitangent =	glm::vec3(0.f);
		}
	
		if (mesh->mTextureCoords[0]) // if vectex has texture coord data
			v.m_texcoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y); // load data
		else 
			v.m_texcoords = glm::vec2(0.f); // else load empty vector 

		m_vertices[i] = v; // push vertex to array
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++) { // for every face

		aiFace face = mesh->mFaces[i]; // get face
		for (size_t j = 0; j < face.mNumIndices; j++) { //get indeices from face
			m_indices.push_back(face.mIndices[j]); // store indices in std::vector
		}
	}

}
