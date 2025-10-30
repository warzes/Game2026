#include "stdafx.h"
#include "RPPostFrame.h"
#include "NanoIO.h"
#include "NanoOpenGL3.h"
#include "NanoLog.h"
#include "NanoRenderMesh.h"
//=============================================================================
bool RPPostFrame::Init(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;

	m_program = LoadShaderProgram("data/shaders/postFrame/vertex.glsl", "data/shaders/postFrame/fragment.glsl");
	if (!m_program)
	{
		Fatal("Scene Main RenderPass Shader failed!");
		return false;
	}

	glUseProgram(m_program);
	SetUniform(GetUniformLocation(m_program, "frameTexture"), 0);

	m_fbo = { std::make_unique<Framebuffer>(true, false, true) };
	m_fbo->AddAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);

	std::vector<QuadVertex> vertices = {
		{glm::vec2(-1.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
		{glm::vec2(-1.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
		{glm::vec2( 1.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
		{glm::vec2( 1.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
		{glm::vec2( 1.0f,  1.0f), glm::vec2(1.0f, 1.0f)},
		{glm::vec2(-1.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
	};

	GLuint currentVBO = GetCurrentBuffer(GL_ARRAY_BUFFER);
	m_vbo = CreateBuffer(GL_ARRAY_BUFFER, BufferUsage::Static, vertices.size() * sizeof(QuadVertex), vertices.data());
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	SetQuadVertexAttributes();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, currentVBO);

	glUseProgram(0);

	SamplerInfo samperCI{};
	samperCI.minFilter = TextureFilter::Nearest;
	samperCI.magFilter = TextureFilter::Nearest;
	m_sampler = CreateSamplerState(samperCI);

	return true;
}
//=============================================================================
void RPPostFrame::Close()
{
	glDeleteProgram(m_program);
	m_fbo.reset();
}
//=============================================================================
void RPPostFrame::Resize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	if (m_framebufferWidth == framebufferWidth && m_framebufferHeight == framebufferHeight)
		return;

	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_fbo->UpdateAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);
}
//=============================================================================
void RPPostFrame::Draw(Framebuffer* preFBO)
{
	m_fbo->Bind();
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(m_program);
	glBindSampler(0, m_sampler);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, preFBO->GetAttachments()[0].id);
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindSampler(0, 0);
}
//=============================================================================