#pragma once

// Render Pass DirectionalLightsShadowMap

#include "Framebuffer.h"

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

struct GameWorldData;

class RenderPass1 final
{
public:
	bool Init(ShadowQuality shadowQuality);
	void Close();

	void Draw(const GameWorldData& worldData);

	void SetShadowQuality(ShadowQuality quality);

	const auto& GetDepthFBO() const { return m_depthFBO; }
	const auto& GetProjection() const { return m_orthoProjection; }

	void BindDepthTexture(size_t id, unsigned slot) const;
	const glm::mat4& GetLightSpaceMatrix(size_t id) const { return m_lightSpaceMatrix[id]; }

private:
	bool initProgram();
	bool initFBO();
	void drawScene(const glm::mat4& lightSpaceMatrix, const GameWorldData& worldData);

	ProgramHandle                                m_program{ 0 };
	int                                          m_mvpMatrixId{ -1 };
	int                                          m_hasDiffuseMapId{ -1 };

	ShadowQuality                                m_shadowQuality;
	float                                        m_orthoDimension;
	glm::mat4                                    m_orthoProjection; // for directional lights

	std::array<Framebuffer, MaxDirectionalLight> m_depthFBO;
	std::array<glm::mat4, MaxDirectionalLight>   m_lightSpaceMatrix;
};