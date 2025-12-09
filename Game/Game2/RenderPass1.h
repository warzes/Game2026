#pragma once

class GameDirectionalLight;
class GamePointLight;

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

/*
TODO: под каждую карту теней свой FBO. это возможно не эффективно.
попробовать один FBO c атачем нужных текстур
или попробовать атачитить массив текстур и выбирать нужное через glFramebufferTextureLayer или glFramebufferTexture с параметром layer 
*/

class RenderPass1 final
{
public:
	bool Init(ShadowQuality shadowQuality);
	void Close();

	void RenderShadows(const GameWorldData& worldData);

	void SetShadowQuality(ShadowQuality quality);

	const auto& GetDepthFBO() const { return m_depthFBODirLights; }

	void BindDirLightDepthTexture(size_t id, unsigned slot) const;
	const glm::mat4& GetLightSpaceMatrix(size_t id) const { return m_lightSpaceMatrix[id]; }

private:
	bool initProgram();
	bool initFBO();
	void drawScene(GameDirectionalLight* currentLight, const GameWorldData& worldData);
	void drawScene(GamePointLight* currentLight, const GameWorldData& worldData);
	void drawMesh(const Mesh& mesh, int hasDiffuseMapId);

	ShadowQuality                                m_shadowQuality;
	glm::mat4                                    m_pointLightProj;  // for point lights
	float                                        m_shadowFarPlane{ 100.0f };

	ProgramHandle                                m_programDirLight{ 0 };
	int                                          m_dirLightMvpMatrixId{ -1 };
	int                                          m_dirLightHasDiffuseMapId{ -1 };

	ProgramHandle                                m_programPointLight{ 0 };
	int                                          m_pointLightModelMatrixId{ -1 };
	int                                          m_pointLightCubeMatricesId[6] = { -1 };
	int                                          m_pointLightLightPosId{ -1 };
	int                                          m_pointLightFarPlaneId{ -1 };
	int                                          m_pointLightHasDiffuseMapId{ -1 };

	std::array<Framebuffer, MaxDirectionalLight> m_depthFBODirLights;
	std::array<glm::mat4, MaxDirectionalLight>   m_lightSpaceMatrix;

	std::array<Framebuffer, MaxPointLight>       m_depthFBOPointLights;
};

class OldRenderPass1 final
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