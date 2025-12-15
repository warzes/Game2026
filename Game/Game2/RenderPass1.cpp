#include "stdafx.h"
#include "RenderPass1.h"
#include "GameScene.h"
//=============================================================================
bool RenderPass1::Init(ShadowQuality shadowQuality)
{
	m_shadowQuality = shadowQuality;
	m_pointLightProj = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, m_shadowFarPlane);

	if (!initProgram())
		return false;

	if (!initFBO())
		return false;

	return true;
}
//=============================================================================
void RenderPass1::Close()
{
	if (m_programDirLight.handle)
		glDeleteProgram(m_programDirLight.handle);
	if (m_programPointLight.handle)
		glDeleteProgram(m_programPointLight.handle);

	for (size_t i = 0; i < m_depthFBODirLights.size(); i++)
	{
		m_depthFBODirLights[i].Destroy();
	}

	for (size_t i = 0; i < m_depthFBOPointLights.size(); i++)
	{
		m_depthFBOPointLights[i].Destroy();
	}
}
//=============================================================================
void RenderPass1::RenderShadows(const GameWorldData& worldData)
{
	if (m_shadowQuality == ShadowQuality::Off) return;

	size_t numDirLights = worldData.countGameDirectionalLights;
	if (numDirLights >= m_depthFBODirLights.size())
	{
		Warning("Num Dir Light bigger num");
		numDirLights = m_depthFBODirLights.size() - 1;
	}
	size_t numPointLights = worldData.countGamePointLights;
	if (numPointLights >= m_depthFBOPointLights.size())
	{
		Warning("Num Point Light bigger num");
		numPointLights = m_depthFBOPointLights.size() - 1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glViewport(0, 0, static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));

	glUseProgram(m_programDirLight.handle);
	for (size_t i = 0; i < numDirLights; i++)
	{
		auto* light = worldData.gameDirectionalLights[i];
		if (!light || !light->GetCastShadows() || !light->IsActive()) continue;

		m_depthFBODirLights[i].Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		drawScene(light, worldData);
	}

	glUseProgram(m_programPointLight.handle);
	for (size_t i = 0; i < numPointLights; i++)
	{
		auto* light = worldData.gamePointLights[i];
		if (!light || !light->GetCastShadows() || !light->IsActive()) continue;

		m_depthFBOPointLights[i].Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		drawScene(light, worldData);
	}
}
//=============================================================================
void RenderPass1::SetShadowQuality(ShadowQuality quality)
{
	if (m_shadowQuality == quality) return;

	m_shadowQuality = quality;
	if (m_shadowQuality != ShadowQuality::Off)
	{
		for (size_t i = 0; i < m_depthFBODirLights.size(); i++)
			m_depthFBODirLights[i].Resize(static_cast<uint16_t>(m_shadowQuality), static_cast<uint16_t>(m_shadowQuality));

		for (size_t i = 0; i < m_depthFBOPointLights.size(); i++)
			m_depthFBOPointLights[i].Resize(static_cast<uint16_t>(m_shadowQuality), static_cast<uint16_t>(m_shadowQuality));
	}
}
//=============================================================================
void RenderPass1::drawScene(GameDirectionalLight* currentLight, const GameWorldData& worldData)
{
	for (size_t i = 0; i < worldData.countGameModels; i++)
	{
		if (!worldData.gameModels[i] || !worldData.gameModels[i]->GetData().visible)
			continue;
		if (!worldData.gameModels[i]->IsActive())
			continue;
		if (!worldData.gameModels[i]->GetData().castShadows)
			continue;

		glm::mat4 lightSpaceMatrix = currentLight->GetLightTransformMatrix();

		SetUniform(m_dirLightMvpMatrixId, lightSpaceMatrix * worldData.gameModels[i]->GetTransform()->GetWorldMatrix());

		const auto& meshes = worldData.gameModels[i]->GetData().model.GetMeshes();
		for (const auto& mesh : meshes)
		{
			drawMesh(mesh, m_dirLightHasDiffuseMapId);
		}
	}
}
//=============================================================================
void RenderPass1::drawScene(GamePointLight* currentLight, const GameWorldData& worldData)
{
	const auto& lpos = currentLight->GetPosition();
	glm::mat4 shadowTransforms[] =
	{
		m_pointLightProj * glm::lookAt(lpos, lpos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		m_pointLightProj * glm::lookAt(lpos, lpos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		m_pointLightProj * glm::lookAt(lpos, lpos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		m_pointLightProj * glm::lookAt(lpos, lpos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		m_pointLightProj * glm::lookAt(lpos, lpos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		m_pointLightProj * glm::lookAt(lpos, lpos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	for (size_t i = 0; i < 6; i++)
	{
		SetUniform(m_pointLightCubeMatricesId[i], shadowTransforms[i]);
	}
	SetUniform(m_pointLightLightPosId, lpos);
	SetUniform(m_pointLightFarPlaneId, m_shadowFarPlane);

	for (size_t i = 0; i < worldData.countGameModels; i++)
	{
		if (!worldData.gameModels[i] || !worldData.gameModels[i]->GetData().visible)
			continue;
		if (!worldData.gameModels[i]->IsActive())
			continue;
		if (!worldData.gameModels[i]->GetData().castShadows)
			continue;

		SetUniform(m_pointLightModelMatrixId, worldData.gameModels[i]->GetTransform()->GetWorldMatrix());

		const auto& meshes = worldData.gameModels[i]->GetData().model.GetMeshes();
		for (const auto& mesh : meshes)
		{
			drawMesh(mesh, m_pointLightHasDiffuseMapId);
		}
	}
}
//=============================================================================
void RenderPass1::drawMesh(const Mesh& mesh, int hasDiffuseMapId)
{
	const auto& material = mesh.GetMaterial();
	bool hasDiffuseMap = false;
	Texture2DHandle diffuseTex{ 0 };
	if (material)
	{
		if (!material->diffuseTextures.empty())
		{
			hasDiffuseMap = IsValid(material->diffuseTextures[0]);
			diffuseTex = material->diffuseTextures[0].id;
		}
	}

	SetUniform(hasDiffuseMapId, hasDiffuseMap);
	BindTexture2D(0, diffuseTex);

	mesh.Draw(GL_TRIANGLES);
}
//=============================================================================
void RenderPass1::BindDirLightDepthTexture(size_t id, unsigned slot) const
{
	if (id >= m_depthFBODirLights.size())
	{
		Error("Invalid directional light ID");
		return;
	}
	m_depthFBODirLights[id].BindDepthTexture(slot);
}
//=============================================================================
void RenderPass1::BindPointLightDepthTexture(size_t id, unsigned slot) const
{
	if (id >= m_depthFBOPointLights.size())
	{
		Error("Invalid point light ID");
		return;
	}
	m_depthFBOPointLights[id].BindDepthTexture(slot);
}
//=============================================================================
bool RenderPass1::initProgram()
{
	// DIRECTIONAL SHADER
	{
		m_programDirLight = LoadShaderProgram("data/shaders2/DirLightShadowVert.shader", "data/shaders2/DirLightShadowFrag.shader");
		if (!m_programDirLight.handle)
		{
			Fatal("Scene Shadow Mapping Shader failed!");
			return false;
		}
		glUseProgram(m_programDirLight.handle);

		int diffuseTextureId = GetUniformLocation(m_programDirLight, "diffuseTexture");
		assert(diffuseTextureId > -1);
		SetUniform(diffuseTextureId, 0);

		m_dirLightHasDiffuseMapId = GetUniformLocation(m_programDirLight, "hasDiffuseMap");
		assert(m_dirLightHasDiffuseMapId > -1);
		m_dirLightMvpMatrixId = GetUniformLocation(m_programDirLight, "mvpMatrix");
		assert(m_dirLightMvpMatrixId > -1);
	}

	// POINT SHADER
	{
		m_programPointLight = LoadShaderProgram("data/shaders2/PointLightShadowVert.shader", "data/shaders2/PointLightShadowGeom.shader", "data/shaders2/PointLightShadowFrag.shader");
		if (!m_programPointLight.handle)
		{
			Fatal("Scene Shadow Mapping Shader failed!");
			return false;
		}
		glUseProgram(m_programPointLight.handle);

		int diffuseTextureId = GetUniformLocation(m_programPointLight, "diffuseTexture");
		assert(diffuseTextureId > -1);
		SetUniform(diffuseTextureId, 0);

		m_pointLightHasDiffuseMapId = GetUniformLocation(m_programPointLight, "hasDiffuseMap");
		assert(m_pointLightHasDiffuseMapId > -1);
		m_pointLightModelMatrixId = GetUniformLocation(m_programPointLight, "model");
		assert(m_pointLightModelMatrixId > -1);

		for (size_t i = 0; i < 6; i++)
		{
			m_pointLightCubeMatricesId[i] = GetUniformLocation(m_programPointLight, "cubeMatrices[" + std::to_string(i) + "]");
			assert(m_pointLightCubeMatricesId[i] > -1);
		}

		m_pointLightLightPosId = GetUniformLocation(m_programPointLight, "lightPos");
		assert(m_pointLightLightPosId > -1);

		m_pointLightFarPlaneId = GetUniformLocation(m_programPointLight, "farPlane");
		assert(m_pointLightFarPlaneId > -1);
	}

	glUseProgram(0); // TODO: возможно вернуть прошлую

	return true;
}
//=============================================================================
bool RenderPass1::initFBO()
{
	FramebufferInfo depthFboInfo;
	depthFboInfo.width = static_cast<uint16_t>(m_shadowQuality);
	depthFboInfo.height = static_cast<uint16_t>(m_shadowQuality);

	depthFboInfo.depthAttachment = DepthAttachment{ .type = AttachmentType::Texture };
	for (size_t i = 0; i < m_depthFBODirLights.size(); i++)
	{
		if (!m_depthFBODirLights[i].Create(depthFboInfo))
			return false;
	}

	depthFboInfo.depthAttachment = DepthAttachment{ .type = AttachmentType::TextureCubeMap };
	for (size_t i = 0; i < m_depthFBOPointLights.size(); i++)
	{
		if (!m_depthFBOPointLights[i].Create(depthFboInfo))
			return false;
	}

	return true;
}
//=============================================================================