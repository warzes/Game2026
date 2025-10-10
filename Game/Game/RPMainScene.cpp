#include "stdafx.h"
#include "RPMainScene.h"
#include "GameScene.h"
#include "NanoIO.h"
#include "NanoLog.h"
#include "NanoWindow.h"
https://github.com/Mikepicker/opengl-sandbox/blob/master/res/shaders/ubershader.vs
https://github.com/cforfang/opengl-shadowmapping/blob/master/src/pcf/main.cpp
//=============================================================================
bool RPMainScene::Init(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	m_program = CreateShaderProgram(io::ReadShaderCode("data/shaders/main/vertex.glsl"), io::ReadShaderCode("data/shaders/main/fragment.glsl"));
	if (!m_program)
	{
		Fatal("Scene Main RenderPass Shader failed!");
		return false;
	}

	glUseProgram(m_program);

	m_projectionMatrixId = GetUniformLocation(m_program, "projectionMatrix");
	m_viewMatrixId = GetUniformLocation(m_program, "viewMatrix");
	m_modelMatrixId = GetUniformLocation(m_program, "modelMatrix");
	m_normalMatrixId = GetUniformLocation(m_program, "normalMatrix");

	glUseProgram(0); // TODO: возможно вернуть прошлую

	m_fbo = { std::make_unique<Framebuffer>(true, true, true) };

	m_fbo->AddAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);
	m_fbo->AddAttachment(AttachmentType::RenderBuffer, AttachmentTarget::DepthStencil, m_framebufferWidth, m_framebufferHeight);

	return true;
}
//=============================================================================
void RPMainScene::Close()
{
	m_fbo.reset();
	glDeleteProgram(m_program);
}
//=============================================================================
void RPMainScene::Draw(const std::vector<Entity*>& entites, size_t numEntities)
{
	m_fbo->Bind();
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(m_program);
	SetUniform(m_projectionMatrixId, m_perspective);
	SetUniform(m_viewMatrixId, camera->GetViewMatrix());

	drawScene(entites, numEntities);
}
//=============================================================================
void RPMainScene::Resize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	if (m_framebufferWidth == framebufferWidth && m_framebufferHeight == framebufferHeight)
		return;

	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	m_fbo->UpdateAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);
	m_fbo->UpdateAttachment(AttachmentType::RenderBuffer, AttachmentTarget::DepthStencil, m_framebufferWidth, m_framebufferHeight);
}
//=============================================================================
void RPMainScene::drawScene(const std::vector<Entity*>& entites, size_t numEntities)
{
	ModelDrawInfo drawInfo;
	drawInfo.bindMaterials = true;
	drawInfo.mode = GL_TRIANGLES;
	for (size_t i = 0; i < numEntities; i++)
	{
		SetUniform(m_modelMatrixId, entites[i]->modelMat);
		entites[i]->model.Draw(drawInfo);
	}
}
//=============================================================================