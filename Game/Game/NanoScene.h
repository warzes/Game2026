#pragma once

enum CameraMovement
{
	CameraForward,
	CameraBackward,
	CameraLeft,
	CameraRight,
};

// Default camera values
#ifdef GLM_FORCE_LEFT_HANDED
constexpr float CAMERA_YAW = 90.0f;
#else
constexpr float CAMERA_YAW = -90.0f;
#endif
constexpr float CAMERA_PITCH = 0.0f;
constexpr float CAMERA_SPEED = 2.5f;
constexpr float CAMERA_SENSITIVITY = 0.1f;

class Camera final
{
public:
	Camera(
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = CAMERA_YAW,
		float pitch = CAMERA_PITCH);

	[[nodiscard]] glm::mat4 GetViewMatrix() const;

	void ProcessKeyboard(CameraMovement direction, float deltaTime);
	void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

	void SetPosition(const glm::vec3& position);

	// Attributes
	glm::vec3 Position{ 0.0f };

	glm::vec3 Front{ 0.0f };
	glm::vec3 Up{ 0.0f };
	glm::vec3 Right{ 0.0f };

	glm::vec3 WorldUp{ 0.0f };

	// Euler Angles
	float Yaw{ 0.0f };
	float Pitch{ 0.0f };

	// Options
	float MovementSpeed{ 0.0f };
	float MouseSensitivity{ 0.0f };

private:
	void updateInternal();

	glm::mat4 m_viewMatrix{};
};

struct DirectionalLight final
{
	glm::vec3 direction{ 0.1f };
	glm::vec3 color{ 1.0f };
	float luminosity{ 1.0f }; // TODO: test
};

struct PointLight final
{
	glm::vec3 position{ 0.0f };
	glm::vec3 color{ 1.0f };
	glm::vec3 attenuation{ 1.0f, 0.0f, 0.5f };
	float intensity{ 1.0f }; // TODO: test
};

struct SpotLight final
{
	glm::vec3 position{ 0.0f };
	glm::vec3 direction{ 0.1f };
	glm::vec3 color{ 1.0f };
	glm::vec3 attenuation{ 1.0f, 0.0f, 0.5f };
	float intensity{ 1.0f }; // TODO: test
	float cutOff{ 0.0f };
	float outerCutOff{ 0.0f };
};

struct AmbientBoxLight final
{
	glm::vec3 size{ 1.0f };
	glm::vec3 position{ 0.0f };
	glm::vec3 color{ 1.0f };
	float intensity{ 1.0f };
};

struct AmbientSphereLight final
{
	glm::vec3 position{ 0.0f };
	glm::vec3 color{ 1.0f };
	float intensity{ 1.0f };
	float radius{ 0.5f };
};