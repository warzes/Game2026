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

	/*int textureOffset{ 5 };
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
	SetUniform(GetUniformLocation(m_program, "pointLightCount"), (int)gameData.numPointLights);*/

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
	GLuint albedoTex = 0;

	for (size_t i = 0; i < gameData.numGameObject; i++)
	{
		if (!gameData.gameObjects[i] || !gameData.gameObjects[i]->visible)
			continue;

		SetUniform(m_modelMatrixId, gameData.gameObjects[i]->modelMat);

		const auto& meshes = gameData.gameObjects[i]->model.GetMeshes();
		for (const auto& mesh : meshes)
		{
			const auto& material = mesh.GetMaterial();
			albedoTex = 0;
			if (material)
			{
				albedoTex = material->diffuseTextures[0].id;
			}

			SetUniform(m_hasAlbedoMapId, albedoTex > 0);

			BindTexture2D(0, albedoTex);

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
	};

	m_program = LoadShaderProgram("data/shaders/mainScene/vertex.glsl", "data/shaders/mainScene/fragment.glsl", defines);
	if (!m_program.handle)
	{
		Fatal("Scene Main RenderPass Shader failed!");
		return false;
	}
	glUseProgram(m_program.handle);

	int albedoMap = GetUniformLocation(m_program, "material.albedoMap");
	assert(albedoMap > -1);
	
	SetUniform(albedoMap, 0);

	m_projectionMatrixId = GetUniformLocation(m_program, "projectionMatrix");
	assert(m_projectionMatrixId > -1);
	m_viewMatrixId = GetUniformLocation(m_program, "viewMatrix");
	assert(m_viewMatrixId > -1);
	m_modelMatrixId = GetUniformLocation(m_program, "modelMatrix");
	assert(m_modelMatrixId > -1);

	m_hasAlbedoMapId = GetUniformLocation(m_program, "hasAlbedoMap");
	assert(m_hasAlbedoMapId > -1);
	
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