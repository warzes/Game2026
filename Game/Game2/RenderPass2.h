#pragma once

class RenderPass1;
class OldRenderPass1;
struct GameWorldData;

class RenderPass2 final
{
public:
	bool Init(uint16_t framebufferWidth, uint16_t framebufferHeight);
	void Close();

	void Resize(uint16_t framebufferWidth, uint16_t framebufferHeight);

	void Draw(const RenderPass1& rpShadowMap, const GameWorldData& gameData);

	const Framebuffer& GetFBO() const { return m_fbo; }
	GLuint GetFBOId() const { return m_fbo.GetId(); }
	uint16_t GetWidth() const { return m_framebufferWidth; }
	uint16_t GetHeight() const { return m_framebufferHeight; }

private:
	bool initProgram();
	bool initFBO();
	void setSize(uint16_t framebufferWidth, uint16_t framebufferHeight);
	void drawScene(const GameWorldData& gameData, const glm::mat4& proj, const glm::mat4& view);

	uint16_t      m_framebufferWidth{ 0 };
	uint16_t      m_framebufferHeight{ 0 };
	glm::mat4     m_perspective{ 1.0f };

	ProgramHandle m_program{ 0 };
	int           m_modelMatrixId{ -1 };
	int           m_modelViewMatrixId{ -1 };
	int           m_modelViewProjMatrixId{ -1 };
	int           m_TileUId{ -1 };
	int           m_TileVId{ -1 };

	int           m_colorTexId{ -1 };
	int           m_hasColorTexId{ -1 };
	int           m_normalTexId{ -1 };
	int           m_hasNormalTexId{ -1 };
	int           m_specularTexId{ -1 };
	int           m_hasSpecularTexId{ -1 };
	int           m_glossTexId{ -1 };
	int           m_hasGlossTexId{ -1 };
	int           m_opacityTexId{ -1 };
	int           m_hasOpacityTexId{ -1 };

	Framebuffer   m_fbo;

	SamplerHandle m_sampler{ 0 };
};