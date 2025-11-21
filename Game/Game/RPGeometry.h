#pragma once

#include "Framebuffer.h"

class Camera;
struct GameObjectO;

class RPGeometry final
{
public:
	bool Init(uint16_t framebufferWidth, uint16_t framebufferHeight);
	void Close();

	void Resize(uint16_t framebufferWidth, uint16_t framebufferHeight);

	void Draw(const std::vector<GameObjectO*>& gameObject, size_t numGameObject, Camera* camera);

	const Framebuffer& GetFBO() const { return m_fbo; }
	GLuint GetFBOId() const { return m_fbo.GetId(); }
	uint16_t GetWidth() const { return m_framebufferWidth; }
	uint16_t GetHeight() const { return m_framebufferHeight; }

private:
	void drawScene(const std::vector<GameObjectO*>& gameObject, size_t numGameObject);

	ProgramHandle m_program{ 0 };
	int           m_projectionMatrixId{ -1 };
	int           m_viewMatrixId{ -1 };
	int           m_modelMatrixId{ -1 };
	uint16_t      m_framebufferWidth{ 0 }; // TODO: можно удалить - есть в m_fbo
	uint16_t      m_framebufferHeight{ 0 }; // TODO: можно удалить - есть в m_fbo
	glm::mat4     m_perspective{ 1.0f };

	Framebuffer   m_fbo;
};