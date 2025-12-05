#include "stdafx.h"
#include "RenderPass2.h"
#include "GameScene.h"
#include "NanoLog.h"
#include "NanoWindow.h"
//=============================================================================
bool RenderPass2::Init(uint16_t framebufferWidth, uint16_t framebufferHeight)
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
void RenderPass2::Close()
{
	m_fbo.Destroy();
	glDeleteProgram(m_program.handle);
	glDeleteSamplers(1, &m_sampler);
}
//=============================================================================
void RenderPass2::Draw(const RenderPass1& rpShadowMap, const GameWorldData& gameData)
{
	m_fbo.Bind();
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program.handle);
	SetUniform(m_projectionMatrixId, m_perspective);
	SetUniform(m_viewMatrixId, gameData.camera->GetViewMatrix());

	SetUniform(m_viewPosId, gameData.camera->Position);

	int textureOffset{ 4 };
	for (int i = 0; i < gameData.numDirLights; ++i)
	{
		const auto* light = gameData.dirLights[i];

		std::string prefix = "dirLight[" + std::to_string(i) + "].";
		SetUniform(GetUniformLocation(m_program, prefix + "direction"), light->direction);
		SetUniform(GetUniformLocation(m_program, prefix + "color"), light->color);
		SetUniform(GetUniformLocation(m_program, prefix + "luminosity"), light->luminosity);
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
		SetUniform(GetUniformLocation(m_program, prefix + "attenuation"), light->attenuation);
		SetUniform(GetUniformLocation(m_program, prefix + "intensity"), light->intensity);
	}
	SetUniform(GetUniformLocation(m_program, "pointLightCount"), (int)gameData.numPointLights);

	for (int i = 0; i < gameData.numSpotLights; ++i)
	{
		const auto* light = gameData.spotLights[i];

		std::string prefix = "spotLight[" + std::to_string(i) + "].";
		SetUniform(GetUniformLocation(m_program, prefix + "position"), light->position);
		SetUniform(GetUniformLocation(m_program, prefix + "direction"), light->direction);
		SetUniform(GetUniformLocation(m_program, prefix + "color"), light->color);
		SetUniform(GetUniformLocation(m_program, prefix + "attenuation"), light->attenuation);
		SetUniform(GetUniformLocation(m_program, prefix + "intensity"), light->intensity);
		SetUniform(GetUniformLocation(m_program, prefix + "cutOff"), light->cutOff);
		SetUniform(GetUniformLocation(m_program, prefix + "outerCutOff"), light->outerCutOff);

	}
	SetUniform(GetUniformLocation(m_program, "spotLightCount"), (int)gameData.numSpotLights);

	for (int i = 0; i < gameData.numBoxLights; ++i)
	{
		const auto* light = gameData.boxLights[i];

		std::string prefix = "ambientBoxLight[" + std::to_string(i) + "].";
		SetUniform(GetUniformLocation(m_program, prefix + "size"), light->size);
		SetUniform(GetUniformLocation(m_program, prefix + "position"), light->position);
		SetUniform(GetUniformLocation(m_program, prefix + "color"), light->color);
		SetUniform(GetUniformLocation(m_program, prefix + "intensity"), light->intensity);
	}
	SetUniform(GetUniformLocation(m_program, "ambientBoxLightCount"), (int)gameData.numBoxLights);

	for (int i = 0; i < gameData.numSphereLights; ++i)
	{
		const auto* light = gameData.sphereLights[i];

		std::string prefix = "ambientSphereLight[" + std::to_string(i) + "].";
		SetUniform(GetUniformLocation(m_program, prefix + "position"), light->position);
		SetUniform(GetUniformLocation(m_program, prefix + "color"), light->color);
		SetUniform(GetUniformLocation(m_program, prefix + "intensity"), light->intensity);
		SetUniform(GetUniformLocation(m_program, prefix + "radius"), light->radius);
	}
	SetUniform(GetUniformLocation(m_program, "ambientSphereLightCount"), (int)gameData.numSphereLights);

	glBindSampler(0, m_sampler);
	drawScene(gameData);
	glBindSampler(0, 0);
}
//=============================================================================
void RenderPass2::Resize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	if (m_framebufferWidth == framebufferWidth && m_framebufferHeight == framebufferHeight)
		return;
	setSize(framebufferWidth, framebufferHeight);
	m_fbo.Resize(m_framebufferWidth, m_framebufferHeight);
}
//=============================================================================
void RenderPass2::drawScene(const GameWorldData& gameData)
{
	TextureHandle diffuseTex{ 0 };
	TextureHandle specularTex{ 0 };
	TextureHandle heightMapTex = textures::GetWhiteTexture2D().id;
	TextureHandle normalTex{ 0 };

	// TODO: heightmap textuere

	for (size_t i = 0; i < gameData.numGameObject; i++)
	{
		if (!gameData.gameObjects[i] || !gameData.gameObjects[i]->visible)
			continue;

		SetUniform(m_modelMatrixId, gameData.gameObjects[i]->modelMat);

		const auto& meshes = gameData.gameObjects[i]->model.GetMeshes();
		for (const auto& mesh : meshes)
		{
			const auto& material = mesh.GetMaterial();
			diffuseTex.handle = 0;
			specularTex.handle = 0;
			normalTex.handle = 0;
			if (material)
			{
				if (!material->diffuseTextures.empty())  diffuseTex = material->diffuseTextures[0].id;
				if (!material->specularTextures.empty()) specularTex = material->specularTextures[0].id;
				if (!material->normalTextures.empty())   normalTex = material->normalTextures[0].id;
			}

			SetUniform(m_hasDiffuseMapId, IsValid(diffuseTex));
			SetUniform(m_hasSpecularMapId, IsValid(specularTex));
			SetUniform(m_hasNormalMapId, IsValid(normalTex));

			BindTexture2D(0, diffuseTex);
			BindTexture2D(1, specularTex);
			BindTexture2D(2, heightMapTex);
			BindTexture2D(3, normalTex);


			mesh.Draw(GL_TRIANGLES);
		}
	}
}
//=============================================================================
bool RenderPass2::initProgram()
{
	const std::vector<std::string> defines = {
		"PARALLAX_MAPPING",
		"NORMAL_MAPPING",
		std::string("MAX_DIR_LIGHTS ") + std::to_string(MaxDirectionalLight),
		std::string("MAX_POINT_LIGHTS ") + std::to_string(MaxPointLight),
		std::string("MAX_SPOT_LIGHTS ") + std::to_string(MaxSpotLight),
		std::string("MAX_AMBIENT_BOX_LIGHTS ") + std::to_string(MaxAmbientBoxLight),
		std::string("MAX_AMBIENT_SPHERE_LIGHTS ") + std::to_string(MaxAmbientSphereLight),
	};

	m_program = LoadShaderProgram("data/shaders2/BlinnPhong/vertex.glsl", "data/shaders2/BlinnPhong/fragment.glsl", defines);
	if (!m_program.handle)
	{
		Fatal("Scene Main RenderPass Shader failed!");
		return false;
	}
	glUseProgram(m_program.handle);

	int diffuseMap = GetUniformLocation(m_program, "u_DiffuseMap");
	assert(diffuseMap > -1);
	int specularMap = GetUniformLocation(m_program, "u_SpecularMap");
	assert(specularMap > -1);
	int heightMap = GetUniformLocation(m_program, "u_HeightMap");
	int normalMap = GetUniformLocation(m_program, "u_NormalMap");

		
	SetUniform(diffuseMap, 0);
	SetUniform(specularMap, 1);
	if (heightMap > -1) SetUniform(heightMap, 2);
	if (normalMap > -1) SetUniform(normalMap, 3);

	m_projectionMatrixId = GetUniformLocation(m_program, "projectionMatrix");
	assert(m_projectionMatrixId > -1);
	m_viewMatrixId = GetUniformLocation(m_program, "viewMatrix");
	assert(m_viewMatrixId > -1);
	m_modelMatrixId = GetUniformLocation(m_program, "modelMatrix");
	assert(m_modelMatrixId > -1);
	m_viewPosId = GetUniformLocation(m_program, "viewPos");
	assert(m_viewPosId > -1);

	m_hasDiffuseMapId = GetUniformLocation(m_program, "hasDiffuseMap");
	assert(m_hasDiffuseMapId > -1);
	m_hasSpecularMapId = GetUniformLocation(m_program, "hasSpecularMap");
	//assert(m_hasSpecularMapId > -1);
	m_hasNormalMapId = GetUniformLocation(m_program, "hasNormalMap");
	//assert(m_hasNormalMapId > -1);

	glUseProgram(0); // TODO: возможно вернуть прошлую версию шейдера

	return true;
}
//=============================================================================
bool RenderPass2::initFBO()
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
void RenderPass2::setSize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	const float aspect = (float)m_framebufferWidth / (float)m_framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), aspect, 0.01f, 1000.0f);
}
//=============================================================================