#include "stdafx.h"
#include "RP_1_DirectionalLightsShadowMap.h"
#include "GameScene.h"
#include "NanoIO.h"
#include "NanoLog.h"
//=============================================================================
bool RPDirectionalLightsShadowMap::Init(ShadowQuality shadowQuality)
{
	m_shadowQuality = shadowQuality;
	m_orthoDimension = 10.0f;
	m_orthoProjection = glm::ortho(-m_orthoDimension, m_orthoDimension, -m_orthoDimension, m_orthoDimension, 1.0f, 50.0f);

	if (!initProgram())
		return false;

	if (!initFBO())
		return false;

	return true;
}
//=============================================================================
void RPDirectionalLightsShadowMap::Close()
{
	if (m_program.handle) 
		glDeleteProgram(m_program.handle);

	for (size_t i = 0; i < m_depthFBO.size(); i++)
	{
		m_depthFBO[i].Destroy();
	}
}
//=============================================================================
void RPDirectionalLightsShadowMap::Draw(const GameWorldData& worldData)
{
	if (m_shadowQuality == ShadowQuality::Off) return;
	if (worldData.numDirLights == 0) return;

	size_t numDirLights = worldData.numDirLights;
	if (numDirLights >= m_depthFBO.size())
	{
		Warning("Num Dir Light bigger num");
		numDirLights = m_depthFBO.size() - 1;
	}

	glEnable(GL_DEPTH_TEST);
	glUseProgram(m_program.handle);
	glViewport(0, 0, static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));

	glm::mat4 lightView;
	for (size_t i = 0; i < numDirLights; i++)
	{
		const auto* light = worldData.dirLights[i];
		if (!light) continue;

		m_depthFBO[i].Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::vec3 lightPos = -light->direction * 10.0f;

		//constexpr const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
		//glm::vec3 right = glm::normalize(glm::cross(light->direction, worldUp));
		//glm::vec3 up = glm::normalize(glm::cross(right, light->direction));

		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

		m_lightSpaceMatrix[i] = m_orthoProjection * lightView;
		drawScene(m_lightSpaceMatrix[i], worldData);
	}
}
//=============================================================================
void RPDirectionalLightsShadowMap::SetShadowQuality(ShadowQuality quality)
{
	if (m_shadowQuality == quality) return;

	m_shadowQuality = quality;
	if (m_shadowQuality != ShadowQuality::Off)
	{
		for (size_t i = 0; i < m_depthFBO.size(); i++)
			m_depthFBO[i].Resize(static_cast<uint16_t>(m_shadowQuality), static_cast<uint16_t>(m_shadowQuality));
	}
}
//=============================================================================
void RPDirectionalLightsShadowMap::drawScene(const glm::mat4& lightSpaceMatrix, const GameWorldData& worldData)
{
	for (size_t i = 0; i < worldData.numGameObject; i++)
	{
		if (!worldData.gameObjects[i] || !worldData.gameObjects[i]->visible)
			continue;

		SetUniform((GLuint)m_mvpMatrixId, lightSpaceMatrix * worldData.gameObjects[i]->modelMat);

		const auto& meshes = worldData.gameObjects[i]->model.GetMeshes();
		for (const auto& mesh : meshes)
		{
			const auto& material = mesh.GetPbrMaterial();
			bool hasAlbedoMap = false;
			GLuint albedoTex = 0;
			if (material)
			{
				hasAlbedoMap = material->albedoTexture.id > 0;
				albedoTex = material->albedoTexture.id;
			}

			SetUniform((GLuint)m_hasAlbedoMapId, hasAlbedoMap);
			BindTexture2D(0, albedoTex);

			mesh.Draw(GL_TRIANGLES);
		}
	}
}
//=============================================================================
void RPDirectionalLightsShadowMap::BindDepthTexture(size_t id, unsigned slot) const
{
	m_depthFBO[id].BindDepthTexture(slot);
}
//=============================================================================
bool RPDirectionalLightsShadowMap::initProgram()
{
	m_program = LoadShaderProgram("data/shaders/shadowMapping/vertex.glsl", "data/shaders/shadowMapping/fragment.glsl");
	if (!m_program.handle)
	{
		Fatal("Scene Shadow Mapping Shader failed!");
		return false;
	}
	glUseProgram(m_program.handle);

	int albedoTextureId = GetUniformLocation(m_program, "albedoTexture");
	assert(albedoTextureId > -1);
	m_hasAlbedoMapId = GetUniformLocation(m_program, "hasAlbedoMap");
	assert(m_hasAlbedoMapId > -1);
	m_mvpMatrixId = GetUniformLocation(m_program, "mvpMatrix");
	assert(m_mvpMatrixId > -1);

	SetUniform((GLuint)albedoTextureId, 0);

	glUseProgram(0); // TODO: возможно вернуть прошлую

	return true;
}
//=============================================================================
bool RPDirectionalLightsShadowMap::initFBO()
{
	FramebufferInfo depthFboInfo;
	depthFboInfo.depthAttachment = DepthAttachment{ .type = AttachmentType::Texture };
	depthFboInfo.width = static_cast<uint16_t>(m_shadowQuality);
	depthFboInfo.height = static_cast<uint16_t>(m_shadowQuality);
	for (size_t i = 0; i < m_depthFBO.size(); i++)
	{
		if (!m_depthFBO[i].Create(depthFboInfo))
			return false;
	}

	return true;
}
//=============================================================================