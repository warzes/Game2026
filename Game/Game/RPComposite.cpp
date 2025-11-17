#include "stdafx.h"
#include "RPComposite.h"
#include "NanoIO.h"
#include "NanoOpenGL3Advance.h"
#include "NanoLog.h"
#include "NanoRenderMesh.h"
//=============================================================================
bool RPComposite::Init(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;

	m_program = LoadShaderProgram("data/shaders/composite/vertex.glsl", "data/shaders/composite/fragment.glsl"/*, std::vector<std::string>{"GAMMA_CORRECT"}*/);
	if (!m_program.handle)
	{
		Fatal("Scene Composite RenderPass Shader failed!");
		return false;
	}

	glUseProgram(m_program.handle);
	SetUniform(GetUniformLocation(m_program, "colorInput"), 0);
	SetUniform(GetUniformLocation(m_program, "brightInput"), 1);
	SetUniform(GetUniformLocation(m_program, "ssaoSampler"), 2);
	SetUniform(GetUniformLocation(m_program, "bloom"), false);
	SetUniform(GetUniformLocation(m_program, "useSSAO"), EnableSSAO);

	FramebufferInfo fboInfo;

	fboInfo.colorAttachments.resize(1);
	fboInfo.colorAttachments[0].type = AttachmentType::Texture;
	fboInfo.colorAttachments[0].format = ColorFormat::RGBA;
	fboInfo.colorAttachments[0].dataType = DataType::Float;

	fboInfo.width = m_framebufferWidth;
	fboInfo.height = m_framebufferHeight;

	if (!m_fbo.Create(fboInfo))
		return false;

	std::vector<QuadVertex> vertices = {
		{glm::vec2(-1.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
		{glm::vec2(-1.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
		{glm::vec2( 1.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
		{glm::vec2( 1.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
		{glm::vec2( 1.0f,  1.0f), glm::vec2(1.0f, 1.0f)},
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

	glUseProgram(0);

	return true;
}
//=============================================================================
void RPComposite::Close()
{
	glDeleteProgram(m_program.handle);
	m_fbo.Destroy();
}
//=============================================================================
void RPComposite::Resize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	if (m_framebufferWidth == framebufferWidth && m_framebufferHeight == framebufferHeight)
		return;

	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_fbo.Resize(m_framebufferWidth, m_framebufferHeight);
}
//=============================================================================
void RPComposite::Draw(const Framebuffer* colorFBO, const Framebuffer* SSAOFBO)
{
	m_fbo.Bind();
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(m_program.handle);

	colorFBO->BindColorTexture(0, 0);
	//blurFBO->BindColorTexture(0, 1);

	if (EnableSSAO)
	{
		SSAOFBO->BindColorTexture(0, 2);
	}

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
//=============================================================================