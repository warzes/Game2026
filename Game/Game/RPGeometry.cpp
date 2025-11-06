#include "stdafx.h"
#include "RPGeometry.h"
#include "NanoWindow.h"
#include "NanoLog.h"
#include "GameScene.h"
//=============================================================================
bool RPGeometry::Init(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	m_program = LoadShaderProgram("data/shaders/geometry/vertex.glsl", "data/shaders/geometry/fragment.glsl");
	if (!m_program)
	{
		Fatal("Scene Geometry RenderPass Shader failed!");
		return false;
	}

	glUseProgram(m_program);

	m_projectionMatrixId = GetUniformLocation(m_program, "projectionMatrix");
	m_viewMatrixId = GetUniformLocation(m_program, "viewMatrix");
	m_modelMatrixId = GetUniformLocation(m_program, "modelMatrix");

	glUseProgram(0); // TODO: возможно вернуть прошлую версию шейдера


	FramebufferInfo fboInfo;

	fboInfo.colorAttachments.resize(2);
	fboInfo.colorAttachments[0].type = AttachmentType::Texture;
	fboInfo.colorAttachments[0].format = ColorFormat::RGB;
	fboInfo.colorAttachments[0].dataType = DataType::Float;
	fboInfo.colorAttachments[1].type = AttachmentType::Texture;
	fboInfo.colorAttachments[1].format = ColorFormat::RGB;
	fboInfo.colorAttachments[1].dataType = DataType::Float;

	fboInfo.depthAttachment = DepthAttachment();
	fboInfo.depthAttachment->type = AttachmentType::RenderBuffer;

	fboInfo.width = m_framebufferWidth;
	fboInfo.height = m_framebufferHeight;

	if (!m_fbo.Create(fboInfo))
		return false;

	return true;
}
//=============================================================================
void RPGeometry::Close()
{
	m_fbo.Destroy();
	glDeleteProgram(m_program);
}
//=============================================================================
void RPGeometry::Resize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	if (m_framebufferWidth == framebufferWidth && m_framebufferHeight == framebufferHeight)
		return;

	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	m_fbo.Resize(m_framebufferWidth, m_framebufferHeight);
}
//=============================================================================
void RPGeometry::Draw(const std::vector<GameObject*>& gameObject, size_t numGameObject, Camera* camera)
{
	m_fbo.Bind();
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT/* | GL_STENCIL_BUFFER_BIT*/);

	glUseProgram(m_program);
	SetUniform(m_projectionMatrixId, m_perspective);
	SetUniform(m_viewMatrixId, camera->GetViewMatrix());

	drawScene(gameObject, numGameObject);
}
//=============================================================================
void RPGeometry::drawScene(const std::vector<GameObject*>& gameObject, size_t numGameObject)
{
	ModelDrawInfo drawInfo;
	drawInfo.bindMaterials = true;
	drawInfo.mode = GL_TRIANGLES;
	drawInfo.shaderProgram = m_program;
	for (size_t i = 0; i < numGameObject; i++)
	{
		SetUniform(m_modelMatrixId, gameObject[i]->modelMat);
		gameObject[i]->model.tDraw(drawInfo);
	}
}
//=============================================================================