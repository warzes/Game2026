#include "stdafx.h"
#include "RPDirShadowMap.h"
#include "GameScene.h"
#include "NanoIO.h"
#include "NanoLog.h"
//=============================================================================
bool RPDirShadowMap::Init()
{
	m_shadowQuality = ShadowQuality::Ultra;
	m_bias = 0.0005f;
	m_orthoDimension = 10.0f;
	m_orthoProjection = glm::ortho(-m_orthoDimension, m_orthoDimension, -m_orthoDimension, m_orthoDimension, 0.1f, 100.0f);

	m_program = CreateShaderProgram(LoadShaderCode("data/shaders/shadowMapping/vertex.glsl"), LoadShaderCode("data/shaders/shadowMapping/fragment.glsl"));
	if (!m_program)
	{
		Fatal("Scene Shadow Mapping Shader failed!");
		return false;
	}

	glUseProgram(m_program);
	//SetUniform(GetUniformLocation(m_program, "diffuseTexture"), 0);
	//m_hasDiffuseId = GetUniformLocation(m_program, "hasDiffuse");
	m_projectionMatrixId = GetUniformLocation(m_program, "projectionMatrix");
	m_viewMatrixId = GetUniformLocation(m_program, "viewMatrix");
	m_modelMatrixId = GetUniformLocation(m_program, "modelMatrix");

	SetUniform(m_projectionMatrixId, m_orthoProjection);
	glUseProgram(0); // TODO: возможно вернуть прошлую

	for (size_t i = 0; i < m_depthFBO.size(); i++)
	{
		m_depthFBO[i] = std::make_unique<Framebuffer>(false);
	}
	if (m_shadowQuality != ShadowQuality::Off)
	{
		for (int i{ 0 }; i < m_depthFBO.size(); ++i)
		{
			m_depthFBO[i]->AddAttachment(AttachmentType::Texture, AttachmentTarget::Depth, static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));
		}
	}
	return true;
}
//=============================================================================
void RPDirShadowMap::Close()
{
	glDeleteProgram(m_program);
	for (size_t i = 0; i < m_depthFBO.size(); i++)
	{
		m_depthFBO[i].reset();
	}
}
//=============================================================================
void RPDirShadowMap::Draw(const std::vector<DirectionalLight*>& dirLights, size_t numDirLights, const std::vector<Entity*>& entites, size_t numEntities)
{
	if (m_shadowQuality == ShadowQuality::Off) return;
	if (dirLights.empty()) return;

	glEnable(GL_DEPTH_TEST);
	glUseProgram(m_program);
	glViewport(0, 0, static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));

	for (size_t i = 0; i < numDirLights; i++)
	{
		auto& light = dirLights[i];

		m_depthFBO[i]->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.0f);

		glm::mat4 lightView = glm::lookAt(light->position, light->position + light->direction, glm::vec3(0.0f, 1.0f, 0.0f));
		SetUniform(m_viewMatrixId, lightView);

		drawScene(entites, numEntities);
	}
}
//=============================================================================
void RPDirShadowMap::SetShadowQuality(ShadowQuality quality)
{
	if (m_shadowQuality == quality) return;

	m_shadowQuality = quality;
	if (m_shadowQuality != ShadowQuality::Off)
	{
		for (size_t i = 0; i < m_depthFBO.size(); i++)
			m_depthFBO[i]->UpdateAttachment(AttachmentType::Texture, AttachmentTarget::Depth, static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));
	}
}
//=============================================================================
void RPDirShadowMap::drawScene(const std::vector<Entity*>& entites, size_t numEntities)
{
	ModelDrawInfo drawInfo;
	drawInfo.bindMaterials = false;
	drawInfo.mode = GL_TRIANGLES;
	for (size_t i = 0; i < numEntities; i++)
	{
		SetUniform(m_modelMatrixId, entites[i]->modelMat);
		entites[i]->model.Draw(drawInfo);
	}
}
//=============================================================================