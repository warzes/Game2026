#pragma once

#include "Framebuffer.h"

class Camera;
struct GameObjectO;
class RPDirectionalLightsShadowMap;
struct DirectionalLight;

class RPBlinnPhong final
{
public:
	bool Init(uint16_t framebufferWidth, uint16_t framebufferHeight);
	void Close();

	void Resize(uint16_t framebufferWidth, uint16_t framebufferHeight);

	void Draw(const RPDirectionalLightsShadowMap& rpShadowMap, 
		const std::vector<DirectionalLight*>& dirLights, size_t numDirLights,
		const std::vector<GameObjectO*>& gameObject, size_t numGameObject, 
		Camera* camera);

	const Framebuffer& GetFBO() const { return m_fbo; }
	GLuint GetFBOId() const { return m_fbo.GetId(); }
	uint16_t GetWidth() const { return m_framebufferWidth; }
	uint16_t GetHeight() const { return m_framebufferHeight; }

private:
	void drawScene(const std::vector<GameObjectO*>& gameObject, size_t numGameObject);

	ProgramHandle    m_program{ 0 };
	int       m_projectionMatrixId{ -1 };
	int       m_viewMatrixId{ -1 };
	int       m_modelMatrixId{ -1 };
	int       m_normalMatrixId{ -1 };
	uint16_t  m_framebufferWidth{ 0 }; // TODO: можно удалить - есть в m_fbo
	uint16_t  m_framebufferHeight{ 0 }; // TODO: можно удалить - есть в m_fbo
	glm::mat4 m_perspective{ 1.0f };

	Framebuffer m_fbo;

	SamplerHandle    m_sampler{ 0 };
};