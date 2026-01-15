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

	if (!m_mapGrid.Init())
		return false;

	return true;
}
//=============================================================================
void RenderPass2::Close()
{
	m_mapGrid.Close();
	m_fbo.Destroy();
	glDeleteProgram(m_program.handle);
}
//=============================================================================
void RenderPass2::Draw(const GameWorldData& gameData)
{
	m_fbo.Bind();
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(m_program.handle);
	SetUniform(GetUniformLocation(m_program, "projectionMatrix"), m_perspective);
	SetUniform(GetUniformLocation(m_program, "viewMatrix"), gameData.camera->GetViewMatrix());
	SetUniform(GetUniformLocation(m_program, "viewPos"), gameData.camera->Position);
	
	glBindSampler(0, m_sampler.handle);
	drawScene(gameData);
	glBindSampler(0, 0);

	//glDisable(GL_DEPTH_TEST);
	m_mapGrid.Draw(m_perspective, gameData.camera->GetViewMatrix());
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
	bool hasDiffuseMap = false;
	Texture2DHandle diffuseTex{ 0 };

	for (size_t i = 0; i < gameData.countGameModels; i++)
	{
		if (!gameData.gameModels[i] || !gameData.gameModels[i]->visible)
			continue;

		SetUniform(GetUniformLocation(m_program, "modelMatrix"), gameData.gameModels[i]->modelMat);

		const auto& meshes = gameData.gameModels[i]->model.GetMeshes();
		for (const auto& mesh : meshes)
		{
			const auto& material = mesh.GetMaterial();
			diffuseTex.handle = 0;
			if (material)
			{
				if (!material->diffuseTextures.empty() && IsValid(material->diffuseTextures[0]))
				{
					hasDiffuseMap = true;
					diffuseTex = material->diffuseTextures[0].id;
				}
			}

			SetUniform(GetUniformLocation(m_program, "hasDiffuseTex"), IsValid(diffuseTex));
			BindTexture2D(0, diffuseTex);
			mesh.Draw(GL_TRIANGLES);
		}
	}
}
//=============================================================================
bool RenderPass2::initProgram()
{
	m_program = LoadShaderProgram("data/shaders3/mainVert.shader", "data/shaders3/mainFrag.shader");
	if (!m_program.handle)
	{
		Fatal("Scene Main RenderPass Shader failed!");
		return false;
	}
	glUseProgram(m_program.handle);

	int diffuseMap = GetUniformLocation(m_program, "diffuseTexture");
	assert(diffuseMap > -1);
	SetUniform(diffuseMap, 0);
	
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