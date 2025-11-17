#pragma once

#include "NanoRender.h"
#include "NanoScene.h"
#include "LightO.h"
#include "GridAxis.h"
#include "Framebuffer.h"
#include "SceneRenderPass.h"

enum class SHADOW_QUALITY
{
	OFF = 0,
	TINY = 256,
	SMALL = 512,
	MED = 1024,
	HIGH = 2048,
	ULTRA = 4096
};

struct Entity2 final
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
	void BindEntity(Entity2* ent);

	std::vector<DirectionalLightO>& GetDirectionalLights() { return m_directionalLights; }
	std::vector<SpotLightO>& GetSpotLight() { return m_spotLights; }

	void SetGridAxis(int gridDim);

	void SetShadowQuality(SHADOW_QUALITY quality);

private:
	enum class drawScenePass : uint8_t
	{
		ShadowMapping,
		BlinnPhong
	};

	bool initShadowMappingShader();
	bool initBlinnPhongShader();
	void updateSize();
	void directionalShadowPass();
	void colorMultisamplePass();
	void drawScene(drawScenePass scenePass);

	//GLState                       m_state;

	ProgramHandle                        m_shadowMapping{ 0 };
	int                           m_shadowMappingShaderProjectionMatrixId{ -1 };
	int                           m_shadowMappingShaderViewMatrixId{ -1 };
	int                           m_shadowMappingShaderModelMatrixId{ -1 };

	ProgramHandle                       m_blinnPhong{ 0 };
	int                          m_blinnPhongShaderModelMatrixId{ -1 };
	int                          m_blinnPhongShaderNormalMatrixId{ -1 };
	GLuint                       m_blinnPhongMatrixUBO{ 0 };
	GLuint                       m_blinnPhongMatrixUBOShaderId{ 0 };
	SceneBlinnPhongMatrices      m_blinnPhongMatrix;



	glm::mat4                     m_perspective{ 1.0f };
	uint16_t                      m_framebufferWidth{ 0 };
	uint16_t                      m_framebufferHeight{ 0 };
	Camera*                       m_camera{ nullptr };

	std::vector<Entity2*>         m_entities;
	size_t                        m_maxEnts{ 0 };

	std::vector<DirectionalLightO> m_directionalLights;
	std::vector<SpotLightO>        m_spotLights;

	std::unique_ptr<GridAxis>     m_gridAxis;

	std::unique_ptr<tFramebuffer>                 m_multisample; // color + depth + stencil
	std::unique_ptr<tFramebuffer>                 m_normal; // only color, no multisampling
	std::array<std::unique_ptr<tFramebuffer>, 10> m_stdDepth; // for directional and spotlight shadow mapping

	float m_orthoDimension;
	SHADOW_QUALITY m_shadowQuality;
	float m_bias;
	int m_show_depth_map;
	glm::mat4 m_orthoProjection; // for directional lights

	GLuint m_pbr;
	GLuint m_postProcessing;

	GLuint m_sampler{ 0 };
};