#pragma once

#include "MapGrid.h"

struct GameWorldData;

class RenderPass2 final
{
public:
	bool Init(uint16_t framebufferWidth, uint16_t framebufferHeight);
	void Close();

	void Resize(uint16_t framebufferWidth, uint16_t framebufferHeight);

	void Draw(const GameWorldData& gameData);

	const Framebuffer& GetFBO() const { return m_fbo; }

private:
	bool initProgram();
	bool initFBO();
	void setSize(uint16_t framebufferWidth, uint16_t framebufferHeight);
	void drawScene(const GameWorldData& gameData);

	uint16_t      m_framebufferWidth{ 0 };
	uint16_t      m_framebufferHeight{ 0 };
	glm::mat4     m_perspective{ 1.0f };

	ProgramHandle m_program{ 0 };
	int           m_modelMatrixId{ -1 };

	Framebuffer   m_fbo;

	SamplerHandle m_sampler{ 0 };

	MapGrid m_mapGrid;
};