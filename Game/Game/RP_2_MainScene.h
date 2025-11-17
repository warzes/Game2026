#pragma once

#include "Framebuffer.h"

class RPDirectionalLightsShadowMap;
struct GameWorldData;

class RPMainScene final
{
public:
	bool Init(uint16_t framebufferWidth, uint16_t framebufferHeight);
	void Close();

	void Resize(uint16_t framebufferWidth, uint16_t framebufferHeight);

	void Draw(const RPDirectionalLightsShadowMap& rpShadowMap, const GameWorldData& gameData);

	const Framebuffer& GetFBO() const { return m_fbo; }
	GLuint GetFBOId() const { return m_fbo.GetId(); }
	uint16_t GetWidth() const { return m_framebufferWidth; }
	uint16_t GetHeight() const { return m_framebufferHeight; }

private:
	bool initProgram();
	bool initFBO();
	void setSize(uint16_t framebufferWidth, uint16_t framebufferHeight);
	void drawScene(const GameWorldData& gameData);

	uint16_t  m_framebufferWidth{ 0 };
	uint16_t  m_framebufferHeight{ 0 };
	glm::mat4 m_perspective{ 1.0f };

	ProgramHandle    m_program{ 0 };
	int       m_projectionMatrixId{ -1 };
	int       m_viewMatrixId{ -1 };
	int       m_modelMatrixId{ -1 };
	int       m_camPosId{ -1 };


	int       m_hasAlbedoMapId{ -1 };
	int       m_hasNormalMapId{ -1 };
	int       m_hasMetallicRoughnessMapId{ -1 };
	int       m_hasAOMapId{ -1 };
	int       m_hasEmissiveMapId{ -1 };
	int       m_opacityId{ -1 };

	Framebuffer m_fbo;

	GLuint    m_sampler{ 0 };
};