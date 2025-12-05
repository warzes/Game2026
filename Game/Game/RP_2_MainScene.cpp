#include "stdafx.h"
#include "RP_2_MainScene.h"
#include "GameSceneO.h"
#include "NanoLog.h"
#include "NanoWindow.h"
//=============================================================================
bool RPMainScene::Init(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	setSize(framebufferWidth, framebufferHeight);
	if (!initProgram())
		return false;
	if (!initFBO())
		return false;

	SamplerStateInfo samperCI{};
	samperCI.minFilter = TextureFilter::Nearest;
	samperCI.magFilter = TextureFilter::Nearest;
	m_sampler = CreateSamplerState(samperCI);

	return true;
}
//=============================================================================
void RPMainScene::Close()
{
	m_fbo.Destroy();
	glDeleteProgram(m_program.handle);
	glDeleteSamplers(1, &m_sampler);
}
//=============================================================================
void RPMainScene::Draw(const RPDirectionalLightsShadowMap& rpShadowMap, const GameWorldDataO& gameData)
{
	m_fbo.Bind();
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program.handle);
	SetUniform(m_projectionMatrixId, m_perspective);
	SetUniform(m_viewMatrixId, gameData.camera->GetViewMatrix());
	SetUniform(m_camPosId, gameData.camera->Position);

	int textureOffset{ 5 };
	for (int i = 0; i < gameData.numDirLights; ++i)
	{
		const auto* light = gameData.dirLights[i];

		std::string prefix = "dirLight[" + std::to_string(i) + "].";
		SetUniform(GetUniformLocation(m_program, prefix + "direction"), light->direction);
		SetUniform(GetUniformLocation(m_program, prefix + "color"), light->color);
		SetUniform(GetUniformLocation(m_program, prefix + "depthMap"), textureOffset);
		SetUniform(GetUniformLocation(m_program, prefix + "lightSpaceMatrix"), rpShadowMap.GetLightSpaceMatrix(i));

		rpShadowMap.BindDepthTexture(i, textureOffset);

		textureOffset++;
	}
	SetUniform(GetUniformLocation(m_program, "dirLightCount"), (int)gameData.numDirLights);

	for (int i = 0; i < gameData.numPointLights; ++i)
	{
		const auto* light = gameData.pointLights[i];

		std::string prefix = "pointLight[" + std::to_string(i) + "].";
		SetUniform(GetUniformLocation(m_program, prefix + "position"), light->position);
		SetUniform(GetUniformLocation(m_program, prefix + "color"), light->color);
	}
	SetUniform(GetUniformLocation(m_program, "pointLightCount"), (int)gameData.numPointLights);

	glBindSampler(0, m_sampler);
	drawScene(gameData);
	glBindSampler(0, 0);
}
//=============================================================================
void RPMainScene::Resize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	if (m_framebufferWidth == framebufferWidth && m_framebufferHeight == framebufferHeight)
		return;
	setSize(framebufferWidth, framebufferHeight);
	m_fbo.Resize(m_framebufferWidth, m_framebufferHeight);
}
//=============================================================================
void RPMainScene::drawScene(const GameWorldDataO& gameData)
{
	Texture2DHandle albedoTex{ 0 };
	Texture2DHandle normalTex{ 0 };
	Texture2DHandle metallicRoughnessTex{ 0 };
	Texture2DHandle aoTex{ 0 };
	Texture2DHandle emissiveTex{ 0 };

	for (size_t i = 0; i < gameData.numGameObject; i++)
	{
		if (!gameData.gameObjects[i] || !gameData.gameObjects[i]->visible)
			continue;

		SetUniform(m_modelMatrixId, gameData.gameObjects[i]->modelMat);

		const auto& meshes = gameData.gameObjects[i]->model.GetMeshes();
		for (const auto& mesh : meshes)
		{
			const auto& material = mesh.GetPbrMaterial();
			albedoTex.handle = 0;
			normalTex.handle = 0;
			metallicRoughnessTex.handle = 0;
			aoTex.handle = 0;
			emissiveTex.handle = 0;
			if (material)
			{
				albedoTex = material->albedoTexture.id;
				normalTex = material->normalTexture.id;
				metallicRoughnessTex = material->metallicRoughnessTexture.id;
			}

			SetUniform(m_hasAlbedoMapId, IsValid(albedoTex));
			SetUniform(m_hasNormalMapId, IsValid(normalTex));
			SetUniform(m_hasMetallicRoughnessMapId, IsValid(metallicRoughnessTex));
			SetUniform(m_hasAOMapId, IsValid(aoTex));
			SetUniform(m_hasEmissiveMapId, IsValid(emissiveTex));

			BindTexture2D(0, albedoTex);
			BindTexture2D(1, normalTex);
			BindTexture2D(2, metallicRoughnessTex);
			BindTexture2D(3, aoTex);
			BindTexture2D(4, emissiveTex);

			mesh.Draw(GL_TRIANGLES);
		}
	}
}
//=============================================================================
bool RPMainScene::initProgram()
{
	const std::vector<std::string> defines = { 
		std::string("MAX_DIR_LIGHTS ") + std::to_string(MaxDirectionalLight),
		std::string("MAX_POINT_LIGHTS ") + std::to_string(MaxPointLight),

		std::string("MAX_LIGHTS ") + std::to_string(MaxDirectionalLight /*+ MaxSpotLight + MaxPointLight*/),
	};

	m_program = LoadShaderProgram("data/shaders/blinnPhong/vertex.glsl", "data/shaders/blinnPhong/fragment.glsl", defines);
	if (!m_program.handle)
	{
		Fatal("Scene Main RenderPass Shader failed!");
		return false;
	}
	glUseProgram(m_program.handle);

	int albedoMap = GetUniformLocation(m_program, "material.albedoMap");
	assert(albedoMap > -1);
	int normalMap = GetUniformLocation(m_program, "material.normalMap");
	assert(normalMap > -1);
	int metallicRoughnessMap = GetUniformLocation(m_program, "material.metallicRoughnessMap");
	assert(metallicRoughnessMap > -1);
	int aoMap = GetUniformLocation(m_program, "material.aoMap");
	assert(aoMap > -1);
	int emissiveMap = GetUniformLocation(m_program, "material.emissiveMap");
	assert(emissiveMap > -1);

	m_opacityId = GetUniformLocation(m_program, "material.opacity");
	assert(m_opacityId > -1);

	SetUniform(albedoMap, 0);
	SetUniform(normalMap, 1);
	SetUniform(metallicRoughnessMap, 2);
	SetUniform(aoMap, 3);
	SetUniform(emissiveMap, 4);

	SetUniform(m_opacityId, 1.0f);

	m_projectionMatrixId = GetUniformLocation(m_program, "projectionMatrix");
	assert(m_projectionMatrixId > -1);
	m_viewMatrixId = GetUniformLocation(m_program, "viewMatrix");
	assert(m_viewMatrixId > -1);
	m_modelMatrixId = GetUniformLocation(m_program, "modelMatrix");
	assert(m_modelMatrixId > -1);
	m_camPosId = GetUniformLocation(m_program, "camPos");
	assert(m_camPosId > -1);


	m_hasAlbedoMapId = GetUniformLocation(m_program, "hasAlbedoMap");
	assert(m_hasAlbedoMapId > -1);
	m_hasNormalMapId = GetUniformLocation(m_program, "hasNormalMap");
	assert(m_hasNormalMapId > -1);
	m_hasMetallicRoughnessMapId = GetUniformLocation(m_program, "hasMetallicRoughnessMap");
	assert(m_hasMetallicRoughnessMapId > -1);
	m_hasAOMapId = GetUniformLocation(m_program, "hasAOMap");
	assert(m_hasAOMapId > -1);
	m_hasEmissiveMapId = GetUniformLocation(m_program, "hasEmissiveMap");
	assert(m_hasEmissiveMapId > -1);

	glUseProgram(0); // TODO: возможно вернуть прошлую версию шейдера

	return true;
}
//=============================================================================
bool RPMainScene::initFBO()
{
	FramebufferInfo fboInfo;

	fboInfo.colorAttachments.resize(1);
	fboInfo.colorAttachments[0].type = AttachmentType::Texture;
	fboInfo.colorAttachments[0].format = ColorFormat::RGBA;
	fboInfo.colorAttachments[0].dataType = DataType::UnsignedByte;

	fboInfo.depthAttachment = DepthAttachment();
	fboInfo.depthAttachment->type = AttachmentType::RenderBuffer;

	fboInfo.width = m_framebufferWidth;
	fboInfo.height = m_framebufferHeight;

	if (!m_fbo.Create(fboInfo))
		return false;

	return true;
}
//=============================================================================
void RPMainScene::setSize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	const float aspect = (float)m_framebufferWidth / (float)m_framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), aspect, 0.01f, 1000.0f);
}
//=============================================================================