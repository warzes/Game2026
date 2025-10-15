#include "stdafx.h"
#include "RPMainScene.h"
#include "GameScene.h"
#include "NanoIO.h"
#include "NanoLog.h"
#include "NanoWindow.h"
//https://github.com/Mikepicker/opengl-sandbox/blob/master/res/shaders/ubershader.vs
//https://github.com/cforfang/opengl-shadowmapping/blob/master/src/pcf/main.cpp
//=============================================================================
bool RPMainScene::Init(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	m_program = CreateShaderProgram(io::ReadShaderCode("data/shaders/main/vertex.glsl"), io::ReadShaderCode("data/shaders/main/fragment.glsl"));
	if (!m_program)
	{
		Fatal("Scene Main RenderPass Shader failed!");
		return false;
	}

	glUseProgram(m_program);

	m_projectionMatrixId = GetUniformLocation(m_program, "projectionMatrix");
	m_viewMatrixId = GetUniformLocation(m_program, "viewMatrix");
	m_modelMatrixId = GetUniformLocation(m_program, "modelMatrix");
	m_normalMatrixId = GetUniformLocation(m_program, "normalMatrix");

	glUseProgram(0); // TODO: возможно вернуть прошлую

	m_fbo = { std::make_unique<Framebuffer>(true, true, true) };

	m_fbo->AddAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);
	m_fbo->AddAttachment(AttachmentType::RenderBuffer, AttachmentTarget::DepthStencil, m_framebufferWidth, m_framebufferHeight);

	SamplerInfo samperCI{};
	samperCI.minFilter = TextureFilter::Nearest;
	samperCI.magFilter = TextureFilter::Nearest;
	m_sampler = CreateSamplerState(samperCI);

	return true;
}
//=============================================================================
void RPMainScene::Close()
{
	m_fbo.reset();
	glDeleteProgram(m_program);
	glDeleteSamplers(1, &m_sampler);
}
//=============================================================================
void RPMainScene::Draw(const RPDirShadowMap& rpShadowMap, const std::vector<DirectionalLight*>& dirLights, size_t numDirLights, const std::vector<Entity*>& entites, size_t numEntities, Camera* camera)
{
	m_fbo->Bind();
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(m_program);
	SetUniform(m_projectionMatrixId, m_perspective);
	SetUniform(m_viewMatrixId, camera->GetViewMatrix());

	SetUniform(GetUniformLocation(m_program, "cam.viewPos"), camera->Position);
	SetUniform(GetUniformLocation(m_program, "shadowOn"), 1);
	SetUniform(GetUniformLocation(m_program, "bias"), rpShadowMap.GetBias());

	int textureOffset{ 4 };
	int depthMapIndex{ 0 };
	int lightCount = 0;
	for (int i = lightCount; i < numDirLights; ++i)
	{
		auto& l = dirLights[i];

		SetUniform(GetUniformLocation(m_program, std::string("light[" + std::to_string(i) + "].type")), 0);
		SetUniform(GetUniformLocation(m_program, std::string("light[" + std::to_string(i) + "].position")), l->position);
		SetUniform(GetUniformLocation(m_program, std::string("light[" + std::to_string(i) + "].direction")), l->direction);
		SetUniform(GetUniformLocation(m_program, std::string("light[" + std::to_string(i) + "].ambientStrength")), l->ambientStrength);
		SetUniform(GetUniformLocation(m_program, std::string("light[" + std::to_string(i) + "].diffuseStrength")), l->diffuseStrength);
		SetUniform(GetUniformLocation(m_program, std::string("light[" + std::to_string(i) + "].specularStrength")), l->specularStrength);

		glm::vec3 lightPosition = l->position;
		glm::vec3 lightTarget = lightPosition + l->direction;
		glm::mat4 lightView = glm::lookAt(lightPosition, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
		glActiveTexture(GL_TEXTURE0 + textureOffset);
		glBindTexture(GL_TEXTURE_2D, rpShadowMap.GetDepthFBO()[depthMapIndex]->GetAttachments().at(0).id);
		SetUniform(GetUniformLocation(m_program, "depthMap[" + std::to_string(depthMapIndex) + "]"), textureOffset);
		SetUniform(GetUniformLocation(m_program, "light[" + std::to_string(i) + "].lightSpaceMatrix"), rpShadowMap.GetProjection() * lightView);
		depthMapIndex++;
		textureOffset++;

		lightCount++;
	}

	SetUniform(GetUniformLocation(m_program, "lightCount"), lightCount);

	glBindSampler(0, m_sampler);
	drawScene(entites, numEntities);
}
//=============================================================================
void RPMainScene::Resize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	if (m_framebufferWidth == framebufferWidth && m_framebufferHeight == framebufferHeight)
		return;

	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	m_fbo->UpdateAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);
	m_fbo->UpdateAttachment(AttachmentType::RenderBuffer, AttachmentTarget::DepthStencil, m_framebufferWidth, m_framebufferHeight);
}
//=============================================================================
void RPMainScene::drawScene(const std::vector<Entity*>& entites, size_t numEntities)
{
	ModelDrawInfo drawInfo;
	drawInfo.bindMaterials = true;
	drawInfo.mode = GL_TRIANGLES;
	for (size_t i = 0; i < numEntities; i++)
	{
		SetUniform(m_modelMatrixId, entites[i]->modelMat);
		entites[i]->model.Draw(drawInfo);
	}
}
//=============================================================================