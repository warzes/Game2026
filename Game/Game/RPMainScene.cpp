#include "stdafx.h"
#include "RPMainScene.h"
#include "GameScene.h"
#include "NanoLog.h"
#include "NanoWindow.h"
//=============================================================================
bool RPMainScene::Init(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	const std::vector<std::string> defines = { std::string("MAX_LIGHTS ") + std::to_string(MaxLight) };

	m_program = LoadShaderProgram("data/shaders/pbr/vertex.glsl", "data/shaders/pbr/fragment.glsl", defines);
	if (!m_program)
	{
		Fatal("Scene Main RenderPass Shader failed!");
		return false;
	}
	glUseProgram(m_program);

	m_projectionMatrixId = GetUniformLocation(m_program, "projectionMatrix");
	assert(m_projectionMatrixId > -1);
	m_viewMatrixId = GetUniformLocation(m_program, "viewMatrix");
	assert(m_viewMatrixId > -1);
	m_modelMatrixId = GetUniformLocation(m_program, "modelMatrix");
	assert(m_modelMatrixId > -1);
	m_lightCountId = GetUniformLocation(m_program, "lightCount");
	assert(m_lightCountId > -1);

	SetUniform(GetUniformLocation(m_program, "albedoSampler"), 0);
	SetUniform(GetUniformLocation(m_program, "normalSampler"), 5);

	glUseProgram(0); // TODO: возможно вернуть прошлую версию шейдера

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
	glDeleteProgram(m_program);
	glDeleteSamplers(1, &m_sampler);
}
//=============================================================================
void RPMainScene::Draw(const RPDirShadowMap& rpShadowMap, const std::vector<GameLight*>& lights, size_t numLights, const std::vector<GameObject*>& gameObject, size_t numGameObject, Camera* camera)
{
	m_fbo.Bind();
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT/* | GL_STENCIL_BUFFER_BIT*/);

	glUseProgram(m_program);
	SetUniform(m_projectionMatrixId, m_perspective);
	SetUniform(m_viewMatrixId, camera->GetViewMatrix());

	SetUniform(m_lightCountId, (int)numLights);

	int textureOffset{ 10 };
	for (size_t i = 0; i < numLights; i++)
	{
		const auto* light = lights[i];

		glm::vec3 direction = glm::normalize(-light->position);
		glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

		glm::vec3 right = glm::normalize(glm::cross(direction, worldUp));
		glm::vec3 up = glm::normalize(glm::cross(right, direction));

		glm::mat4 lightProjection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 10.0f);

		glm::mat4 lightView = glm::lookAt(light->position, light->position + direction, up);

		std::string prefix = "lights[" + std::to_string(i) + "].";
		SetUniform(GetUniformLocation(m_program, prefix + "position"), light->position);
		SetUniform(GetUniformLocation(m_program, prefix + "direction"), direction);
		SetUniform(GetUniformLocation(m_program, prefix + "color"), light->color);
		SetUniform(GetUniformLocation(m_program, prefix + "matrix"), lightProjection * lightView);
		SetUniform(GetUniformLocation(m_program, prefix + "shadowMapSampler"), (int)(textureOffset + i));
		//rpShadowMap.GetDepthFBO()[depthMapIndex].BindDepthTexture(textureOffset + i);
		// TODO: shadow map texture - GL_CLAMP_TO_BORDER и borderColor[] = 1.0, 1.0, 1.0, 1.0 через GL_TEXTURE_BORDER_COLOR
	}

	glBindSampler(0, m_sampler);
	drawScene(gameObject, numGameObject);
	glBindSampler(0, 0);
}
//=============================================================================
void RPMainScene::Resize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	if (m_framebufferWidth == framebufferWidth && m_framebufferHeight == framebufferHeight)
		return;

	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	m_fbo.Resize(m_framebufferWidth, m_framebufferHeight);
}
//=============================================================================
void RPMainScene::drawScene(const std::vector<GameObject*>& gameObject, size_t numGameObject)
{
	for (size_t i = 0; i < numGameObject; i++)
	{
		if (!gameObject[i] || !gameObject[i]->visible)
			continue;

		SetUniform(m_modelMatrixId, gameObject[i]->modelMat);

		const auto& meshes = gameObject[i]->model.GetMeshes();
		for (const auto& mesh : meshes)
		{
			const auto& material = mesh.GetPbrMaterial();
			bool hasAlbedo = false;
			GLuint albedoTex = 0;
			bool hasNormal = false;
			GLuint normalTex = 0;
			if (material)
			{
				hasAlbedo = material->albedoTexture.id > 0;
				albedoTex = material->albedoTexture.id;

				hasNormal = material->normalTexture.id > 0;
				normalTex = material->normalTexture.id;
			}

			SetUniform(GetUniformLocation(m_program, "hasAlbedo"), hasAlbedo);
			SetUniform(GetUniformLocation(m_program, "hasNormal"), hasNormal);

			BindTexture2D(0, albedoTex);
			BindTexture2D(5, normalTex);

			mesh.Draw(GL_TRIANGLES);
		}
	}
}
//=============================================================================
