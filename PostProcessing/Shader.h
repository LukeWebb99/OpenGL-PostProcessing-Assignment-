#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

enum TextureUnits { //Enums for assigning textures to designating textures to texture units 

	TU_NULL,

	TU_IRRADIANCE, // Skybox 
	TU_PREFILTER,
	TU_BRDF,

	TU_ALBEDO, // Material
	TU_NORMAL,
	TU_ROUGHNESS,
	TU_AO,
	TU_METALLIC,
	TU_DISPLACEMENT,

	TU_DIRECTIONAL_SHADOW_MAP, // Shadows

	TU_POINT_SHADOW_MAP0,
	TU_POINT_SHADOW_MAP1,
	TU_POINT_SHADOW_MAP2,
	TU_POINT_SHADOW_MAP3,

	TU_SPOT_SHADOW_MAP0,
	TU_SPOT_SHADOW_MAP1,
	TU_SPOT_SHADOW_MAP2,
	TU_SPOT_SHADOW_MAP3,

};

class Shader
{
public:

	Shader();
	~Shader();

	void CreateFromString(const char* vertexShader, const char* fragmentShader);
	void CreateFromFile(const char* vertexPath, const char* geometryShader, const char* fragmentPath);
	void CreateFromFile(const char* vertexPath, const char* fragmentPath);
	void CreateFromFile(const char* computePath);

	std::string ReadFile(const char* path);

	GLuint GetProjectionLocation();
	GLuint GetModelLocation();

	void Bind();
	void Unbind();

	GLuint GetShaderID() { return shaderID; };

	void ClearShader();
	
	void Set1f(GLfloat value, const GLchar* name);
	void Set1i(GLint value, const GLchar* name);
	void Set2f(glm::vec2 value, const GLchar* name);
	void SetVec2f(glm::vec2 value, const GLchar* name);
	void SetVec3f(glm::fvec3 value, const GLchar* name);
	void SetVec4f(glm::vec4 value, const GLchar* name);
	void SetMat3f(glm::mat3 value, const char* name, bool transpose);
	void SetMat4f(glm::mat4 value, const char* uniformName, bool transpose);

	void Validate();
	void QueryWorkgroups();

private:

	GLuint shaderID;
	GLuint u_Projection, u_Model;

	void CompileShader(const char* vertexShader, const char* fragmentShader);
	void CompileShader(const char* vertexShader, const char* geometryShader, const char* fragmentShader);
	void CompileShader(const char* computeShader);
	void AddShader(GLuint program, const char* shaderCode, GLenum type);

};

