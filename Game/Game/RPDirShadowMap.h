#pragma once

#include "Framebuffer.h"
#include "NanoScene.h"

constexpr size_t MaxDirectionalLight = 16;

enum class ShadowQuality 
{
	Off = 0,
	Tiny = 256,
	Small = 512,
	Med = 1024,
	High = 2048,
	Ultra = 4096,
	Mega = 8192
};

struct GameObject;

class RPDirShadowMap final
{
public:
	bool Init();
	void Close();

	void Draw(
		const std::vector<DirectionalLight*>& dirLights, size_t numDirLights,
		const std::vector<GameObject*>& gameObject, size_t numGameObject);

	void SetShadowQuality(ShadowQuality quality);
	float GetBias() const { return m_bias; }

	const auto& GetDepthFBO() const { return m_depthFBO; }
	const auto& GetProjection() const { return m_orthoProjection; }

private:
	void drawScene(const std::vector<GameObject*>& gameObject, size_t numGameObject);
	GLuint        m_program{ 0 };
	int           m_projectionMatrixId{ -1 };
	int           m_viewMatrixId{ -1 };
	int           m_modelMatrixId{ -1 };
	//int           m_hasDiffuseId{ -1 };

	ShadowQuality m_shadowQuality;
	float         m_bias;
	float         m_orthoDimension;
	glm::mat4     m_orthoProjection; // for directional lights

	std::array<Framebuffer, MaxDirectionalLight> m_depthFBO;
};