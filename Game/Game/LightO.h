#pragma once

#include "NanoRender.h"

enum class LightTypeO : uint8_t
{
	Directional,
	Spot
};

class LightO
{
public:
	LightO(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec);
	virtual ~LightO();

	virtual void Draw() = 0;
	
	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 pos);

	glm::vec3 GetAmbientStrength();
	glm::vec3 GetDiffuseStrength();
	glm::vec3 GetSpecularStrength();
	void SetAmbientStrength(glm::vec3 c);
	void SetDiffuseStrength(glm::vec3 c);
	void SetSpecularStrength(glm::vec3 c);
	void SetModelMatrix(glm::mat4 m);
	void SetViewMatrix(glm::mat4 m);
	void SetProjMatrix(glm::mat4 m);

	virtual LightTypeO GetType() = 0;

protected:
	glm::vec3 m_ambientStrength;
	glm::vec3 m_diffuseStrength;
	glm::vec3 m_specularStrength;

	glm::vec3 m_position;
	glm::mat4 m_model;
	glm::mat4 m_view;
	glm::mat4 m_proj;

	GLuint m_vao;
	GLuint m_vbo;

	Texture2D m_icon;
};

class DirectionalLightO final : public LightO
{
public:
	DirectionalLightO(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 dir);
	void Draw() final;
	void Draw(float orthoDim);

	glm::vec3 GetDirection();
	void SetDirection(glm::vec3 dir);

	LightTypeO GetType() final;
private:
	glm::vec3 m_direction;
	GLuint    m_shaderIcon;
	GLuint    m_shaderDirection;
};

class SpotLightO final : public LightO
{
public:
	SpotLightO(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 dir, float innerAngle, float outerAngle);
	
	void Draw() final;

	glm::vec3 GetDirection();
	void SetDirection(glm::vec3 dir);
	float GetCutOff();
	void SetCutOff(float cutoff);
	float GetOuterCutOff();
	void SetOuterCutOff(float out);

	LightTypeO GetType() final;
private:
	glm::vec3 m_direction;
	float     m_cutOff; // radians
	float     m_outerCutOff; // radians
	GLuint    m_shaderIcon;
	GLuint    m_shaderCutOff;
};