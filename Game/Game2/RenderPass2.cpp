#include "stdafx.h"
#include "RenderPass2.h"
#include "GameScene.h"
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
}
//=============================================================================
void RenderPass2::Draw(const RenderPass1& rpShadowMap, const GameWorldData& gameData)
{
	m_fbo.Bind();
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);

	glUseProgram(m_program.handle);

	SetUniform(m_TileUId, 1.0f);
	SetUniform(m_TileVId, 1.0f);

	// Set materials
	SetUniform(GetUniformLocation(m_program, "material.opacity"), 1.0f);
	SetUniform(GetUniformLocation(m_program, "material.baseColor"), glm::vec3(1.0f));
	SetUniform(GetUniformLocation(m_program, "material.specularity"), 1.0f);
	SetUniform(GetUniformLocation(m_program, "material.shininess"), 10.0f);

	// set other uniforms
	SetUniform(GetUniformLocation(m_program, "shadowsFarPlane"), rpShadowMap.GetShadowFarPlane());
	SetUniform(GetUniformLocation(m_program, "ambientStrength"), 0.1f);
	SetUniform(GetUniformLocation(m_program, "ambientColor"), glm::vec3(1.0f));
	SetUniform(GetUniformLocation(m_program, "alphaTest"), true);

	// TODO: skybox

	int textureOffset{ 6 };
	// Set Directional Lights
	SetUniform(GetUniformLocation(m_program, "directionalLightsNumber"), (int)gameData.countGameDirectionalLights);
	for (size_t i = 0; i < gameData.countGameDirectionalLights; i++)
	{
		auto* light = gameData.gameDirectionalLights[i];
		const std::string idStr = std::to_string(i);

		glm::vec3 dir = gameData.oldCamera->GetViewMatrix() * glm::vec4(light->GetDirection(), 0.0f);

		SetUniform(GetUniformLocation(m_program, "directionalLights[" + idStr + "].dir"), dir);
		SetUniform(GetUniformLocation(m_program, "directionalLights[" + idStr + "].color"), light->GetColor());
		SetUniform(GetUniformLocation(m_program, "directionalLights[" + idStr + "].intensity"), light->GetIntensity());
		SetUniform(GetUniformLocation(m_program, "directionalLights[" + idStr + "].castShadows"), light->GetCastShadows());
		if (light->GetCastShadows())
		{
			rpShadowMap.BindDirLightDepthTexture(i, textureOffset);
			SetUniform(GetUniformLocation(m_program, "directionalLights[" + idStr + "].shadowMap"), textureOffset);
			SetUniform(GetUniformLocation(m_program, "directionalLights[" + idStr + "].lightViewProj"), light->GetLightTransformMatrix());
			textureOffset++;
		}
	}

	// Set Spot Lights
	// TODO:

	// Set Point Lights
	SetUniform(GetUniformLocation(m_program, "pointsLightsNumber"), (int)gameData.countGamePointLights);
	for (size_t i = 0; i < gameData.countGamePointLights; i++)
	{
		auto* light = gameData.gamePointLights[i];
		const std::string idStr = std::to_string(i);

		glm::vec3 view = gameData.oldCamera->GetViewMatrix() * glm::vec4(light->GetPosition(), 0.0f);

		SetUniform(GetUniformLocation(m_program, "pointLights[" + idStr + "].pos"), view);
		SetUniform(GetUniformLocation(m_program, "pointLights[" + idStr + "].modelPos"), light->GetPosition());
		SetUniform(GetUniformLocation(m_program, "pointLights[" + idStr + "].color"), light->GetColor());
		SetUniform(GetUniformLocation(m_program, "pointLights[" + idStr + "].intensity"), light->GetIntensity());

		//SetUniform(GetUniformLocation(m_program, "pointLights[" + idStr + "].constant"), light->GetConstant());
		//SetUniform(GetUniformLocation(m_program, "pointLights[" + idStr + "].linear"), light->GetLinear());
		//SetUniform(GetUniformLocation(m_program, "pointLights[" + idStr + "].quadratic"), light->GetQuadratic());
		//SetUniform(GetUniformLocation(m_program, "pointLights[" + idStr + "].att"), light->GetAtt());
		
		SetUniform(GetUniformLocation(m_program, "pointLights[" + idStr + "].castShadows"), light->GetCastShadows());
		if (light->GetCastShadows())
		{
			rpShadowMap.BindPointLightDepthTexture(i, textureOffset);
			SetUniform(GetUniformLocation(m_program, "pointLights[" + idStr + "].shadowMap"), textureOffset);
			textureOffset++;
		}
	}


	glBindSampler(0, m_sampler.handle);
	drawScene(gameData, m_perspective, gameData.oldCamera->GetViewMatrix());
	glBindSampler(0, 0);
}
//=============================================================================
void RenderPass2::Resize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	if (m_framebufferWidth == framebufferWidth && m_framebufferHeight == framebufferHeight)
		return;
	if (framebufferWidth == 0 || framebufferHeight == 0)
		return;
	setSize(framebufferWidth, framebufferHeight);
	m_fbo.Resize(m_framebufferWidth, m_framebufferHeight);
}
//=============================================================================
void RenderPass2::drawScene(const GameWorldData& gameData, const glm::mat4& proj, const glm::mat4& view)
{
	bool hasDiffuseMap = false;
	Texture2DHandle diffuseTex{ 0 };
	bool hasSpecularMap = false;
	Texture2DHandle specularTex{ 0 };
	bool hasGlossMap = false;
	Texture2DHandle glossTex{ 0 };
	bool hasNormalMap = false;
	Texture2DHandle normalTex{ 0 };
	bool hasOpacityMap = false;
	Texture2DHandle opacityTex{ 0 };

	for (size_t i = 0; i < gameData.countGameModels; i++)
	{
		if (!gameData.gameModels[i] || !gameData.gameModels[i]->GetData().visible)
			continue;
		if (!gameData.gameModels[i]->IsActive())
			continue;

		SetUniform(GetUniformLocation(m_program, "material.receiveShadows"), gameData.gameModels[i]->GetData().receiveShadows);

		SetUniform(m_modelMatrixId, gameData.gameModels[i]->GetTransform()->GetWorldMatrix());
		SetUniform(m_modelViewMatrixId, view * gameData.gameModels[i]->GetTransform()->GetWorldMatrix());
		SetUniform(m_modelViewProjMatrixId, proj * view * gameData.gameModels[i]->GetTransform()->GetWorldMatrix());

		const auto& meshes = gameData.gameModels[i]->GetData().model.GetMeshes();
		for (const auto& mesh : meshes)
		{
			const auto& material = mesh.GetMaterial();
			hasDiffuseMap = false;
			diffuseTex.handle = 0;
			hasSpecularMap = false;
			specularTex.handle = 0;
			hasGlossMap = false;
			glossTex.handle = 0;
			hasNormalMap = false;
			normalTex.handle = 0;
			hasOpacityMap = false;
			opacityTex.handle = 0;

			if (material)
			{
				if (!material->diffuseTextures.empty() && IsValid(material->diffuseTextures[0]))
				{
					hasDiffuseMap = true;
					diffuseTex = material->diffuseTextures[0].id;
				}
				if (!material->specularTextures.empty() && IsValid(material->specularTextures[0]))
				{
					hasSpecularMap = true;
					specularTex = material->specularTextures[0].id;
				}
				//if (!material->.empty() && IsValid(material->[0]))
				{
					//hasGlossMap = true;
					//glossTex = material->[0].id;
				}
				if (!material->normalTextures.empty() && IsValid(material->normalTextures[0]))
				{
					hasNormalMap = true;
					normalTex = material->normalTextures[0].id;
				}
				//if (!material->.empty() && IsValid(material->[0]))
				{
					//hasOpacityMap = true;
					//opacityTex = material->[0].id;
				}
			}

			SetUniform(m_hasColorTexId, hasDiffuseMap);
			BindTexture2D(0, diffuseTex);

			SetUniform(m_hasNormalTexId, hasNormalMap);
			BindTexture2D(1, normalTex);

			SetUniform(m_hasSpecularTexId, hasSpecularMap);
			BindTexture2D(2, specularTex);

			SetUniform(m_hasGlossTexId, hasGlossMap);
			BindTexture2D(3, glossTex);

			SetUniform(m_hasOpacityTexId, hasOpacityMap);
			BindTexture2D(4, opacityTex);

			mesh.Draw(GL_TRIANGLES);
		}
	}
}
//=============================================================================
bool RenderPass2::initProgram()
{
	const std::vector<std::string> defines = {
		std::string("MAX_DIR_LIGHTS ") + std::to_string(MaxDirectionalLight),
		std::string("MAX_POINT_LIGHTS ") + std::to_string(MaxPointLight),
		std::string("MAX_SPOT_LIGHTS ") + std::to_string(MaxSpotLight),
		std::string("MAX_AMBIENT_BOX_LIGHTS ") + std::to_string(MaxAmbientBoxLight),
		std::string("MAX_AMBIENT_SPHERE_LIGHTS ") + std::to_string(MaxAmbientSphereLight),
	};

	m_program = LoadShaderProgram("data/shaders2/BlinnPhong/vertexNew.shader", "data/shaders2/BlinnPhong/fragmentNew.shader", defines);
	if (!m_program.handle)
	{
		Fatal("Scene Main RenderPass Shader failed!");
		return false;
	}
	glUseProgram(m_program.handle);

	// texture bind slots
	{
		m_hasColorTexId = GetUniformLocation(m_program, "material.hasColorTex");
		assert(m_hasColorTexId > -1);
		m_colorTexId = GetUniformLocation(m_program, "material.colorTex");
		assert(m_colorTexId > -1);
		SetUniform(m_colorTexId, 0);

		m_hasNormalTexId = GetUniformLocation(m_program, "material.hasNormalTex");
		assert(m_hasNormalTexId > -1);
		m_normalTexId = GetUniformLocation(m_program, "material.normalTex");
		assert(m_normalTexId > -1);
		SetUniform(m_normalTexId, 1);

		m_hasSpecularTexId = GetUniformLocation(m_program, "material.hasSpecularTex");
		assert(m_hasSpecularTexId > -1);
		m_specularTexId = GetUniformLocation(m_program, "material.specularTex");
		assert(m_specularTexId > -1);
		SetUniform(m_specularTexId, 2);

		m_hasGlossTexId = GetUniformLocation(m_program, "material.hasGlossTex");
		assert(m_hasGlossTexId > -1);
		m_glossTexId = GetUniformLocation(m_program, "material.glossTex");
		assert(m_glossTexId > -1);
		SetUniform(m_glossTexId, 3);

		m_hasOpacityTexId = GetUniformLocation(m_program, "material.hasOpacityTex");
		assert(m_hasOpacityTexId > -1);
		m_opacityTexId = GetUniformLocation(m_program, "material.opacityTex");
		assert(m_opacityTexId > -1);
		SetUniform(m_opacityTexId, 4);
	}

	// vertex uniforms slots
	m_modelMatrixId = GetUniformLocation(m_program, "modelMatrix");
	assert(m_modelMatrixId > -1);
	m_modelViewMatrixId = GetUniformLocation(m_program, "modelViewMatrix");
	assert(m_modelViewMatrixId > -1);
	m_modelViewProjMatrixId = GetUniformLocation(m_program, "modelViewProjMatrix");
	assert(m_modelViewProjMatrixId > -1);
	m_TileUId = GetUniformLocation(m_program, "TileU");
	assert(m_TileUId > -1);
	m_TileVId = GetUniformLocation(m_program, "TileV");
	assert(m_TileVId > -1);

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
	assert(m_framebufferHeight > 0);

	const float aspect = (float)m_framebufferWidth / (float)m_framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), aspect, 0.01f, 1000.0f);
}
//=============================================================================