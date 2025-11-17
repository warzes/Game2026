#include "stdafx.h"
#include "RPSSAOBlur.h"
#include "NanoWindow.h"
#include "NanoLog.h"
#include "GameScene.h"
// TODO: в каждом renderpass создается свой квад, а нужно сделать общий
//=============================================================================
bool RPSSAOBlur::Init(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;

	m_program = LoadShaderProgram("data/shaders/ssaoBlur/vertex.glsl", "data/shaders/ssaoBlur/fragment.glsl");
	if (!m_program.handle)
	{
		Fatal("Scene SSAO Blur RenderPass Shader failed!");
		return false;
	}

	glUseProgram(m_program.handle);
	SetUniform(GetUniformLocation(m_program, "ssaoInput"), 0);

	std::vector<QuadVertex> vertices = {
		{glm::vec2(-1.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
		{glm::vec2(-1.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
		{glm::vec2(1.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
		{glm::vec2(1.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
		{glm::vec2(1.0f,  1.0f), glm::vec2(1.0f, 1.0f)},
		{glm::vec2(-1.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
	};

	GLuint currentVBO = GetCurrentBuffer(GL_ARRAY_BUFFER);
	m_vbo = CreateBuffer(BufferType::ArrayBuffer, BufferUsage::StaticDraw, vertices.size() * sizeof(QuadVertex), vertices.data());
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo.handle);
	QuadVertex::SetVertexAttributes();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, currentVBO);

	glUseProgram(0); // TODO: возможно вернуть прошлую версию шейдера

	FramebufferInfo fboInfo;

	fboInfo.colorAttachments.resize(1);
	fboInfo.colorAttachments[0].type = AttachmentType::Texture;
	fboInfo.colorAttachments[0].format = ColorFormat::Red;
	fboInfo.colorAttachments[0].dataType = DataType::Float;

	fboInfo.width = m_framebufferWidth;
	fboInfo.height = m_framebufferHeight;

	if (!m_fbo.Create(fboInfo))
		return false;

	return true;
}
//=============================================================================
void RPSSAOBlur::Close()
{
	m_fbo.Destroy();
	glDeleteProgram(m_program.handle);
}
//=============================================================================
void RPSSAOBlur::Resize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	if (m_framebufferWidth == framebufferWidth && m_framebufferHeight == framebufferHeight)
		return;

	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;

	m_fbo.Resize(m_framebufferWidth, m_framebufferHeight);
}
//=============================================================================
void RPSSAOBlur::Draw(const Framebuffer* preFBO)
{
	m_fbo.Bind();
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_program.handle);

	preFBO->BindColorTexture(0, 0);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
//=============================================================================