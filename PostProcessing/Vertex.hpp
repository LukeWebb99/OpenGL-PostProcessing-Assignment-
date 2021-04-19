#pragma once

#define MAX_BONE_WEIGHTS 4

struct Vertex
{
	glm::vec3 m_position;
	glm::vec2 m_texcoords;
	glm::vec3 m_normal;
	glm::vec3 m_tangent;
	glm::vec3 m_bitangent;
	int m_boneIDs[MAX_BONE_WEIGHTS];
	float m_weights[MAX_BONE_WEIGHTS];
};