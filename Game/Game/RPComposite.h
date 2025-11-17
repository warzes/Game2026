#pragma once

#include "Framebuffer.h"

class RPComposite final
{
public:
	bool Init(uint16_t framebufferWidth, uint16_t framebufferHeight);
	void Close();

	void Resize(uint16_t framebufferWidth, uint16_t framebufferHeight);

	void Draw(const Framebuffer* preColorFBO, const Framebuffer* SSAOFBO);

	const Framebuffer& GetFBO() const { return m_fbo; }
	GLuint GetFBOId() const { return m_fbo.GetId(); }
	uint16_t GetWidth() const { return m_framebufferWidth; }
	uint16_t GetHeight() const { return m_framebufferHeight; }
private:
	Framebuffer m_fbo;
	ProgramHandle                       m_program{ 0 };
	GLuint                       m_vao{ 0 };
	GLuint                       m_vbo{ 0 };
	uint16_t                     m_framebufferWidth{ 0 }; // TODO: можно удалить - есть в m_fbo
	uint16_t                     m_framebufferHeight{ 0 }; // TODO: можно удалить - есть в m_fbo
};