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

	m_program = LoadShaderProgram("data/shaders/shadowMapping/vertex.glsl", "data/shaders/shadowMapping/fragment.glsl");
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
		DepthAttachment depth;
		depth.type = AttachmentType::Texture;

		FramebufferInfo fboInfo;
		fboInfo.depthAttachment = depth;
		fboInfo.width = static_cast<int>(m_shadowQuality);
		fboInfo.height = static_cast<int>(m_shadowQuality);

		if (!m_depthFBO[i].Create(fboInfo))
			return false;
	}

	return true;
}
//=============================================================================
void RPDirShadowMap::Close()
{
	glDeleteProgram(m_program);
	for (size_t i = 0; i < m_depthFBO.size(); i++)
	{
		m_depthFBO[i].Destroy();
	}
}
//=============================================================================
void RPDirShadowMap::Draw(const std::vector<DirectionalLight*>& dirLights, size_t numDirLights, const std::vector<GameObject*>& gameObject, size_t numGameObject)
{
	if (m_shadowQuality == ShadowQuality::Off) return;
	if (dirLights.empty() || numDirLights == 0) return;

	glEnable(GL_DEPTH_TEST);
	glUseProgram(m_program);
	glViewport(0, 0, static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));

	for (size_t i = 0; i < numDirLights; i++)
	{
		if (numDirLights >= m_depthFBO.size())
		{
			Warning("Num Dir Light bigger num");
			break;
		}

		const auto& light = dirLights[i];

		m_depthFBO[i].Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		constexpr glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
		glm::vec3 right = glm::normalize(glm::cross(light->direction, worldUp));
		glm::vec3 up = glm::normalize(glm::cross(right, light->direction));

		glm::mat4 lightView = glm::lookAt(light->position, light->position + light->direction, up);
		SetUniform(m_viewMatrixId, lightView);

		drawScene(gameObject, numGameObject);
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
			m_depthFBO[i].Resize(static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));
	}
}
//=============================================================================
void RPDirShadowMap::drawScene(const std::vector<GameObject*>& gameObject, size_t numGameObject)
{
	ModelDrawInfo drawInfo;
	drawInfo.bindMaterials = false;
	drawInfo.mode = GL_TRIANGLES;
	for (size_t i = 0; i < numGameObject; i++)
	{
		SetUniform(m_modelMatrixId, gameObject[i]->modelMat);
		gameObject[i]->model.tDraw(drawInfo);
	}
}
//=============================================================================