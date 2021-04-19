#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed, float fov);

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange, GLfloat deltaTime);

	glm::vec3 GetCameraPosition();
	glm::vec3 getCameraDirection();

	GLfloat GetYaw();
	GLfloat GetPitch();

	float GetFOV() { return m_fov; };
	float* GetFOVptr() { return &m_fov; };

	glm::mat4 CalculateViewMatrix();
	glm::mat4 CalculateProjectionMatrix(GLfloat bufferWidth, GLfloat bufferHeight);

	~Camera();

private:
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldUp;

	GLfloat m_yaw;
	GLfloat m_pitch;

	float m_fov;

	GLfloat moveSpeed;
	GLfloat startingMoveSpeed;
	GLfloat turnSpeed;

	void update();
};

