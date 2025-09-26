#include "stdafx.h"
#include "Camera.h"

CameraO2::CameraO2() {}

CameraO2::CameraO2(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void CameraO2::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

	if (keys[GLFW_KEY_EQUAL])
	{
		position += front * velocity;
	}

	if (keys[GLFW_KEY_MINUS])
	{
		position -= front * velocity;
	}

	if (keys[GLFW_KEY_LEFT])
	{
		position -= right * velocity;
	}

	if (keys[GLFW_KEY_RIGHT])
	{
		position += right * velocity;
	}

	if (keys[GLFW_KEY_UP])
	{
		position.y += 0.1;
	}

	if (keys[GLFW_KEY_DOWN])
	{
		position.y -= 0.1;
	}
}

void CameraO2::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}

glm::mat4 CameraO2::calculateViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::vec3 CameraO2::getCameraPosition()
{
	return position;
}
glm::vec3 CameraO2::getCameraDirection()
{
	return glm::normalize(front);
}

void CameraO2::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}


CameraO2::~CameraO2()
{
}
