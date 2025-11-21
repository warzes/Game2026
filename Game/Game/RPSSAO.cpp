#include "stdafx.h"
#include "RPSSAO.h"
#include "NanoWindow.h"
#include "NanoLog.h"
#include "GameSceneO.h"
// TODO: в каждом renderpass создается свой квад, а нужно сделать общий
//=============================================================================
bool RPSSAO::Init(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);
	glm::vec2 size((float)m_framebufferWidth, (float)m_framebufferHeight);
	m_noiseScale = size / 4.0f;

	m_program = LoadShaderProgram("data/shaders/ssao/vertex.glsl", "data/shaders/ssao/fragment.glsl");
	if (!m_program.handle)
	{
		Fatal("Scene SSAO RenderPass Shader failed!");
		return false;
	}

	glUseProgram(m_program.handle);
	SetUniform(GetUniformLocation(m_program, "gPosition"), 0);
	SetUniform(GetUniformLocation(m_program, "gNormal"), 1);
	SetUniform(GetUniformLocation(m_program, "texNoise"), 2);

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

	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
	std::default_random_engine generator;
	for (GLuint i = 0; i < 64; ++i)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0;

		scale = glm::lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		m_ssaoKernel.push_back(sample);
	}

	std::vector<glm::vec3> ssaoNoise;
	for (GLuint i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise.push_back(noise);
	}

	GLint currentTexture = GetCurrentTexture(GL_TEXTURE_2D);
	glGenTextures(1, &m_noiseTexture);
	glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return true;
}
//=============================================================================
void RPSSAO::Close()
{
	m_fbo.Destroy();
	glDeleteProgram(m_program.handle);
}
//=============================================================================
void RPSSAO::Resize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	if (m_framebufferWidth == framebufferWidth && m_framebufferHeight == framebufferHeight)
		return;

	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);
	glm::vec2 size((float)m_framebufferWidth, (float)m_framebufferHeight);
	m_noiseScale = size / 4.0f;

	m_fbo.Resize(m_framebufferWidth, m_framebufferHeight);
}
//=============================================================================
void RPSSAO::Draw(const Framebuffer* preFBO)
{
	m_fbo.Bind();
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_program.handle);
	SetUniform(GetUniformLocation(m_program, "samples"), m_ssaoKernel);
	SetUniform(GetUniformLocation(m_program, "noiseScale"), m_noiseScale);
	SetUniform(GetUniformLocation(m_program, "projection"), m_perspective);

	preFBO->BindColorTexture(0, 0);
	preFBO->BindColorTexture(1, 1);
	BindTexture2D(2, m_noiseTexture);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
//=============================================================================