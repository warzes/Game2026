#pragma once

#include "NanoRender.h"
#include "NanoScene.h"
#include "Light.h"
#include "GridAxis.h"
#include "Framebuffer.h"

enum class SHADOW_QUALITY
{
	OFF = 0,
	TINY = 256,
	SMALL = 512,
	MED = 1024,
	HIGH = 2048,
	ULTRA = 4096
};

enum class SHADOW_METHOD
{
	LANCE_WILLIAMS,
	SECOND_DEPTH
};

struct Entity final
{
	const AABB& GetAABB() const noexcept { return model.GetAABB(); }

	Model     model;
	glm::mat4 modelMat{ glm::mat4(1.0f) };
	bool      visible{ true };
};

class Scene final
{
public:
	bool Init();
	void Close();
	void Draw();

	void BindCamera(Camera* camera);
	void BindEntity(Entity* ent);

	std::vector<DirectionalLight>& GetDirectionalLights() { return m_directionalLights; }
	std::vector<SpotLight>& GetSpotLight() { return m_spotLights; }

	void SetGridAxis(int gridDim);

	void SetShadowQuality(SHADOW_QUALITY quality);

private:
	enum class drawScenePass : uint8_t
	{
		ShadowMapping,
		BlinnPhong,
		Temp
	};

	bool initMainShader();
	bool initShadowMappingShader();
	bool initBlinnPhongShader();
	void updateSize();
	void directionalShadowPass();
	void colorMultisamplePass();
	void drawScene(drawScenePass scenePass);

	//GLState                       m_state;

	GLuint                        m_mainShader{ 0 };
	int                           m_mainShaderProjectionMatrixId{ -1 };
	int                           m_mainShaderViewMatrixId{ -1 };
	int                           m_mainShaderModelMatrixId{ -1 };
	int                           m_mainShaderNormalMatrixId{ -1 };

	GLuint                        m_shadowMapping{ 0 };
	int                           m_shadowMappingShaderProjectionMatrixId{ -1 };
	int                           m_shadowMappingShaderViewMatrixId{ -1 };
	int                           m_shadowMappingShaderModelMatrixId{ -1 };

	GLuint                        m_blinnPhong{ 0 };
	int                           m_blinnPhongShaderProjectionMatrixId{ -1 };
	int                           m_blinnPhongShaderViewMatrixId{ -1 };
	int                           m_blinnPhongShaderModelMatrixId{ -1 };
	int                           m_blinnPhongShaderNormalMatrixId{ -1 };

	glm::mat4                     m_perspective{ 1.0f };
	uint16_t                      m_framebufferWidth{ 0 };
	uint16_t                      m_framebufferHeight{ 0 };
	Camera*                       m_camera{ nullptr };

	std::vector<Entity*>          m_entities;
	size_t                        m_maxEnts{ 0 };

	std::vector<DirectionalLight> m_directionalLights;
	std::vector<SpotLight>        m_spotLights;

	std::unique_ptr<GridAxis>     m_gridAxis;

	std::unique_ptr<Framebuffer>                 m_multisample; // color + depth + stencil
	std::unique_ptr<Framebuffer>                 m_normal; // only color, no multisampling
	std::array<std::unique_ptr<Framebuffer>, 10> m_stdDepth; // for directional and spotlight shadow mapping

	float m_orthoDimension;
	SHADOW_QUALITY m_shadowQuality;
	SHADOW_METHOD m_shadowMethod;
	float m_bias;
	int m_show_depth_map;
	glm::mat4 m_orthoProjection; // for directional lights

	GLuint m_pbr;
	GLuint m_postProcessing;

};