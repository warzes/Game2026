#pragma once

#include "Framebuffer.h"

class RPSSAO final
{
public:
	bool Init(uint16_t framebufferWidth, uint16_t framebufferHeight);
	void Close();

	void Resize(uint16_t framebufferWidth, uint16_t framebufferHeight);

	void Draw(const Framebuffer* preFBO);

	const Framebuffer& GetFBO() const { return m_fbo; }
	GLuint GetFBOId() const { return m_fbo.GetId(); }
	uint16_t GetWidth() const { return m_framebufferWidth; }
	uint16_t GetHeight() const { return m_framebufferHeight; }

private:
	GLuint                       m_program{ 0 };
	uint16_t                     m_framebufferWidth{ 0 }; // TODO: можно удалить - есть в m_fbo
	uint16_t                     m_framebufferHeight{ 0 }; // TODO: можно удалить - есть в m_fbo
	glm::mat4 m_perspective{ 1.0f };
	GLuint                       m_vao{ 0 };
	GLuint                       m_vbo{ 0 };

	Framebuffer m_fbo;

	std::vector<glm::vec3> m_ssaoKernel;
	glm::vec2 m_noiseScale;
	GLuint m_noiseTexture;
};