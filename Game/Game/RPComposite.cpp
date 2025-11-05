#include "stdafx.h"
#include "RPComposite.h"
#include "NanoIO.h"
#include "NanoOpenGL3.h"
#include "NanoLog.h"
#include "NanoRenderMesh.h"
//=============================================================================
bool RPComposite::Init(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;

	m_program = LoadShaderProgram("data/shaders/composite/vertex.glsl", "data/shaders/composite/fragment.glsl"/*, std::vector<std::string>{"GAMMA_CORRECT"}*/);
	if (!m_program)
	{
		Fatal("Scene Composite RenderPass Shader failed!");
		return false;
	}

	glUseProgram(m_program);
	SetUniform(GetUniformLocation(m_program, "colorInput"), 0);
	SetUniform(GetUniformLocation(m_program, "brightInput"), 1);
	SetUniform(GetUniformLocation(m_program, "ssaoSampler"), 2);
	SetUniform(GetUniformLocation(m_program, "bloom"), false);
	SetUniform(GetUniformLocation(m_program, "useSSAO"), EnableSSAO);


	m_fbo = { std::make_unique<Framebuffer>(true, false, true) };
	m_fbo->AddAttachment(AttachmentType::Texture, AttachmentTarget::ColorRGBA, m_framebufferWidth, m_framebufferHeight);

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

	return true;
}
//=============================================================================
void RPComposite::Close()
{
	glDeleteProgram(m_program);
	m_fbo.reset();
}
//=============================================================================
void RPComposite::Resize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	if (m_framebufferWidth == framebufferWidth && m_framebufferHeight == framebufferHeight)
		return;

	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_fbo->UpdateAttachment(AttachmentType::Texture, AttachmentTarget::ColorRGBA, m_framebufferWidth, m_framebufferHeight);
}
//=============================================================================
void RPComposite::Draw(Framebuffer* colorFBO, Framebuffer* SSAOFBO)
{
	m_fbo->Bind();
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(m_program);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorFBO->GetAttachments()[0].id);
	
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, blurFBO->GetAttachments()[0].id);

	if (EnableSSAO)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, SSAOFBO->GetAttachments()[0].id);
	}

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
//=============================================================================