#include "stdafx.h"
#include "Scene.h"
#include "NanoWindow.h"
#include "NanoIO.h"
#include "NanoLog.h"

//пройти по точечному источнику света и случаю без текстур
//отрефакторить шейдер убрав условия, вынеся в начало vec diffuseColor и подобное которое берет или текстуру или материал

//=============================================================================
bool Scene::Init()
{
	m_framebufferWidth = window::GetWidth();
	m_framebufferHeight = window::GetHeight();
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	m_entities.reserve(100000);
		
	//m_state.depthState.enable = true;
	//m_state.blendState.enable = true;
	//m_state.blendState.srcAlpha = BlendFactor::OneMinusSrcAlpha;

	m_shadowQuality = SHADOW_QUALITY::ULTRA;
	m_shadowMethod = SHADOW_METHOD::LANCE_WILLIAMS;
	m_bias = 0.0005f;
	m_show_depth_map = 0;
	m_orthoDimension = 10.0f;
	m_orthoProjection = glm::ortho(-m_orthoDimension, m_orthoDimension, -m_orthoDimension, m_orthoDimension, 0.1f, 100.0f);

	//m_pbr = CreateShaderProgram(io::ReadShaderCode("data/shaders/PBR/vertex.glsl"), io::ReadShaderCode("data/shaders/PBR/fragment.glsl"));
	//m_postProcessing = CreateShaderProgram(io::ReadShaderCode("data/shaders/post_processing/vertex.glsl"), io::ReadShaderCode("data/shaders/post_processing/fragment.glsl"));
	
	if (!initShadowMappingShader())
		return false;
	if (!initBlinnPhongShader())
		return false;
	if (!initDefferedShader())
		return false;

	m_multisample = { std::make_unique<Framebuffer>(true, true, true) };

	m_normal = { std::make_unique<Framebuffer>(true, false, true) };
	m_stdDepth = {
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false)
	};

	m_multisample->AddAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);
	m_multisample->AddAttachment(AttachmentType::RenderBuffer, AttachmentTarget::DepthStencil, m_framebufferWidth, m_framebufferHeight);
	m_normal->AddAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);

	if (m_shadowQuality != SHADOW_QUALITY::OFF)
	{
		for (int i{ 0 }; i < 10; ++i)
		{
			m_stdDepth.at(i)->AddAttachment(AttachmentType::Texture, AttachmentTarget::Depth, static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));
		}
	}
	return true;
}
//=============================================================================
void Scene::Close()
{
	m_gridAxis.reset();
}
//=============================================================================
void Scene::BindCamera(Camera* camera)
{
	m_camera = camera;
}
//=============================================================================
void Scene::BindEntity(Entity* ent)
{
	if (m_maxEnts >= m_entities.size())
		m_entities.push_back(ent);
	else
		m_entities[m_maxEnts] = ent;

	m_maxEnts++;
}
//=============================================================================
void Scene::Draw()
{
	updateSize();

	if (m_typeRender == SceneTypeRender::Deffered)
	{
		// gBuffer render
		{
			// setup GL state.
			glEnable(GL_DEPTH_TEST);
			glDepthMask(true);
			glDisable(GL_BLEND);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			//glEnable(GL_CULL_FACE);
			//glFrontFace(GL_CCW);

			//
			// In the first pass, we just write to the gbuffer.
			//
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_gBufferFBO); // bind g buffer for writing.

			glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
			glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(m_outputGeoShader); // "output geometry to gbuffer" shader
			SetUniform(glGetUniformLocation(m_outputGeoShader, "projectionMatrix"), m_perspective);
			SetUniform(glGetUniformLocation(m_outputGeoShader, "viewMatrix"), m_camera->GetViewMatrix());

			drawScene(drawScenePass::GBuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		//
		{

			glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
			glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//
			// first, we render a single directional light, with a fullscreen pass.
			//
			glUseProgram(m_directionalLightShader);

			// bind gbuffer textures.
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, colorTexture);

			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, normalTexture);

			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, positionTexture);

			glActiveTexture(GL_TEXTURE0 + 3);
			glBindTexture(GL_TEXTURE_2D, tangentTexture);

			glActiveTexture(GL_TEXTURE0 + 4);
			glBindTexture(GL_TEXTURE_2D, bitangentTexture);

			glUniform3f(glGetUniformLocation(m_directionalLightShader, "uCameraPos"), m_camera->Position.x, m_camera->Position.y, m_camera->Position.z);

			glBindVertexArray(vao);
			// we use attribute-less rendering to render a full-screen triangle.
			// so the triangle vertices are basically stored in the vertex shader.
			// see the vertex shader for more details.
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);
		}

		
	}
	else if (m_typeRender == SceneTypeRender::Forward)
	{
		glEnable(GL_DEPTH_TEST);

		if (m_shadowQuality != SHADOW_QUALITY::OFF)
		{
			// SHADOW PASS : directional & spot light sources
			directionalShadowPass();

			// COLOR PASS : multisampling
			colorMultisamplePass();

			// TEMP blit
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_multisample->GetId());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 0 = default framebuffer (экран)
			glBlitFramebuffer(0, 0, m_framebufferWidth, m_framebufferHeight, 0, 0, m_framebufferWidth, m_framebufferHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			// blit to normal framebuffer (resolve multisampling)

			// bind to default framebuffer

			// draw post processing quad
		}
		else // без теней
		{
		}
	}

	m_maxEnts = 0;
}
//=============================================================================
void Scene::SetGridAxis(int gridDim)
{
	if (gridDim == 0)
		m_gridAxis.reset();
	else
		m_gridAxis = std::make_unique<GridAxis>(gridDim);
}
//=============================================================================
void Scene::SetShadowQuality(SHADOW_QUALITY quality)
{
	m_shadowQuality = quality;
	if (m_shadowQuality != SHADOW_QUALITY::OFF)
	{
		for (int i{ 0 }; i < 10; ++i)
			m_stdDepth.at(i)->UpdateAttachment(AttachmentType::Texture, AttachmentTarget::Depth, static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));
	}
}
//=============================================================================
bool Scene::initShadowMappingShader()
{
	m_shadowMapping = CreateShaderProgram(io::ReadShaderCode("data/shaders/shadowMapping/vertex.glsl"), io::ReadShaderCode("data/shaders/shadowMapping/fragment.glsl"));
	if (!m_shadowMapping)
	{
		Fatal("Scene Shadow Mapping Shader failed!");
		return false;
	}

	glUseProgram(m_shadowMapping);
	SetUniform(GetUniformLocation(m_shadowMapping, "diffuseTexture"), 0);
	SetUniform(GetUniformLocation(m_shadowMapping, "hasDiffuse"), 1);
	m_shadowMappingShaderProjectionMatrixId = GetUniformLocation(m_shadowMapping, "projectionMatrix");
	m_shadowMappingShaderViewMatrixId = GetUniformLocation(m_shadowMapping, "viewMatrix");
	m_shadowMappingShaderModelMatrixId = GetUniformLocation(m_shadowMapping, "modelMatrix");
	glUseProgram(0);
	return true;
}
//=============================================================================
bool Scene::initBlinnPhongShader()
{
	m_blinnPhong = CreateShaderProgram(io::ReadShaderCode("data/shaders/blinn_phong/vertex.glsl"), io::ReadShaderCode("data/shaders/blinn_phong/fragment.glsl"));
	if (!m_blinnPhong)
	{
		Fatal("Scene Blinn Phong Shader failed!");
		return false;
	}

	glUseProgram(m_blinnPhong);
	SetUniform(GetUniformLocation(m_blinnPhong, "diffuseTexture"), 0);
	SetUniform(GetUniformLocation(m_blinnPhong, "specularTexture"), 1);
	SetUniform(GetUniformLocation(m_blinnPhong, "normalTexture"), 2);
	for (size_t i = 0; i < 10; i++)
	{
		SetUniform(GetUniformLocation(m_blinnPhong, "depthMap[" + std::to_string(i) + "]"), 4+(int)i);
	}

	m_blinnPhongShaderProjectionMatrixId = GetUniformLocation(m_blinnPhong, "projectionMatrix");
	m_blinnPhongShaderViewMatrixId = GetUniformLocation(m_blinnPhong, "viewMatrix");
	m_blinnPhongShaderModelMatrixId = GetUniformLocation(m_blinnPhong, "modelMatrix");
	m_blinnPhongShaderNormalMatrixId = GetUniformLocation(m_blinnPhong, "normalMatrix");
	glUseProgram(0);

	return true;
}
//=============================================================================
bool Scene::initDefferedShader()
{
	glGenVertexArrays(1, &vao);

	{
		m_outputGeoShader = CreateShaderProgram(io::ReadShaderCode("data/shaders/deffered/gBuffer.vert"), io::ReadShaderCode("data/shaders/deffered/gBuffer.frag"));
		if (!m_outputGeoShader)
		{
			Fatal("Scene Geo Shader failed!");
			return false;
		}

		glUseProgram(m_outputGeoShader);
		SetUniform(GetUniformLocation(m_outputGeoShader, "diffuseTexture"), 0);
		glUseProgram(0);
	}

	{
		m_directionalLightShader = CreateShaderProgram(io::ReadShaderCode("data/shaders/deffered/directionalLight.vert"), io::ReadShaderCode("data/shaders/deffered/directionalLight.frag"));
		if (!m_directionalLightShader)
		{
			Fatal("Scene Directional Light Shader failed!");
			return false;
		}

		glUseProgram(m_directionalLightShader);
		SetUniform(GetUniformLocation(m_directionalLightShader, "uColorTex"), 0);
		SetUniform(GetUniformLocation(m_directionalLightShader, "uNormalTex"), 1);
		SetUniform(GetUniformLocation(m_directionalLightShader, "uPositionTex"), 2);
		SetUniform(GetUniformLocation(m_directionalLightShader, "uTangentTex"), 3);
		SetUniform(GetUniformLocation(m_directionalLightShader, "uBitangentTex"), 4);

		glUseProgram(0);
	}

	{
		glGenFramebuffers(1, &m_gBufferFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBO);

		// RGBA8 color texture-p
		glGenTextures(1, &colorTexture);
		glBindTexture(GL_TEXTURE_2D, colorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_framebufferWidth, m_framebufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//  RGBA16F normal texture.
		glGenTextures(1, &normalTexture);
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_framebufferWidth, m_framebufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//  RGBA16F position texture.
		glGenTextures(1, &positionTexture);
		glBindTexture(GL_TEXTURE_2D, positionTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_framebufferWidth, m_framebufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, positionTexture, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//  RGBA16F tangent texture.
		glGenTextures(1, &tangentTexture);
		glBindTexture(GL_TEXTURE_2D, tangentTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_framebufferWidth, m_framebufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, tangentTexture, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//  RGBA16F bitangent texture.
		glGenTextures(1, &bitangentTexture);
		glBindTexture(GL_TEXTURE_2D, bitangentTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_framebufferWidth, m_framebufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, bitangentTexture, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		// we need a z-buffer for the gbuffer. but we don't need to read from it.
		// so instead create a renderbuffer.
		glGenRenderbuffers(1, &depthRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, m_framebufferWidth, m_framebufferHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

		// specify that we can render to all three attachments.
		// this is very important! It won't work otherwise.
		GLenum tgts[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(5, tgts);

		// make sure nothing went wrong:
		GLenum status;
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			printf("Framebuffer not complete. Status: %d", status);
			exit(1);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	return true;
}
//=============================================================================
void Scene::updateSize()
{
	if (m_framebufferWidth != window::GetWidth() || m_framebufferHeight != window::GetHeight())
	{
		m_framebufferWidth = window::GetWidth();
		m_framebufferHeight = window::GetHeight();
		m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

		m_multisample->UpdateAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);
		m_multisample->UpdateAttachment(AttachmentType::RenderBuffer, AttachmentTarget::DepthStencil, m_framebufferWidth, m_framebufferHeight);
		m_normal->UpdateAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);
	}
}
//=============================================================================
void Scene::directionalShadowPass()
{
	glViewport(0, 0, static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));

	glUseProgram(m_shadowMapping);
	SetUniform(m_shadowMappingShaderProjectionMatrixId, m_orthoProjection);

	// render directional depth maps
	int sLightsOffset{ 0 };
	for (int i{ 0 }; i < m_directionalLights.size(); ++i, ++sLightsOffset)
	{
		auto& dlight = m_directionalLights[i];

		m_stdDepth[i]->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.0f);

		glm::vec3 lightPosition = dlight.GetPosition();
		glm::vec3 lightTarget = dlight.GetDirection();
		glm::mat4 lightView = glm::lookAt(lightPosition, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
		SetUniform(m_shadowMappingShaderViewMatrixId, lightView);

		// draw scene
		drawScene(drawScenePass::ShadowMapping);
	}

	for (int i{ 0 }; i < m_spotLights.size(); ++i)
	{
		auto& slight = m_spotLights[i];

		m_stdDepth[sLightsOffset + i]->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::vec3 lightPosition = slight.GetPosition();
		glm::vec3 lightDirection = slight.GetDirection();
		glm::vec3 lightTarget = lightPosition + lightDirection;
		glm::vec3 up = (lightDirection == glm::vec3(0.0f, -1.0f, 0.0f)) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 lightView = glm::lookAt(lightPosition, lightTarget, up);
		float outerCutOff = slight.GetOuterCutOff();

		glm::mat4 spotProj = glm::perspective(
			outerCutOff * 2.0f, 1.0f,
			0.01f,
			1000.f
		);

		SetUniform(m_shadowMappingShaderProjectionMatrixId, spotProj);
		SetUniform(m_shadowMappingShaderViewMatrixId, lightView);

		// draw scene
		// TODO: доделать
		if (m_shadowMethod == SHADOW_METHOD::LANCE_WILLIAMS)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			drawScene(drawScenePass::ShadowMapping);
		}
		else
		{
			//if (solid)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
			}
			//else
			//{
			//	glDisable(GL_CULL_FACE);
			//}
			drawScene(drawScenePass::ShadowMapping);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_CULL_FACE);
}
//=============================================================================
void Scene::colorMultisamplePass()
{
	m_multisample->Bind();
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(m_blinnPhong);
	SetUniform(m_blinnPhongShaderProjectionMatrixId, m_perspective);
	SetUniform(m_blinnPhongShaderViewMatrixId, m_camera->GetViewMatrix());
	SetUniform(GetUniformLocation(m_blinnPhong, "cam.viewPos"), m_camera->Position);

	SetUniform(GetUniformLocation(m_blinnPhong, "shadowOn"), 1);
	SetUniform(GetUniformLocation(m_blinnPhong, "shadowMethod"), static_cast<int>(m_shadowMethod));
	SetUniform(GetUniformLocation(m_blinnPhong, "bias"), m_bias);

	// shader set light
	{
		int dCount = m_directionalLights.size();
		int sCount = m_spotLights.size();
		int lightCount = dCount + sCount;
		int dOffset{ 0 };
		int sOffset{ dCount };
		SetUniform(GetUniformLocation(m_blinnPhong, "lightCount"), lightCount);

		for (int i{ 0 }; i < m_directionalLights.size(); ++i)
		{
			auto& l = m_directionalLights[i];

			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + dOffset) + "].type")), static_cast<int>(l.GetType()));
			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + dOffset) + "].position")), l.GetPosition());
			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + dOffset) + "].direction")), l.GetDirection());
			// if BLINN_PHONG
			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + dOffset) + "].ambientStrength")), l.GetAmbientStrength());
			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + dOffset) + "].diffuseStrength")), l.GetDiffuseStrength());
			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + dOffset) + "].specularStrength")), l.GetSpecularStrength());
			// elif PBR
			//SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + dOffset) + "].color")), l.GetDiffuseStrength());
		}
		for (int i{ 0 }; i < m_spotLights.size(); ++i)
		{
			auto& l = m_spotLights[i];

			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + sOffset) + "].type")), static_cast<int>(l.GetType()));
			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + sOffset) + "].cutOff")), cosf(l.GetCutOff()));
			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + sOffset) + "].outerCutOff")), cosf(l.GetOuterCutOff()));
			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + sOffset) + "].position")), l.GetPosition());
			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + sOffset) + "].direction")), l.GetDirection());
			// if BLINN_PHONG
			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + sOffset) + "].ambientStrength")), l.GetAmbientStrength());
			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + sOffset) + "].diffuseStrength")), l.GetDiffuseStrength());
			SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + sOffset) + "].specularStrength")), l.GetSpecularStrength());
			// elif PBR
			//SetUniform(GetUniformLocation(m_blinnPhong, std::string("light[" + std::to_string(i + sOffset) + "].color")), sLights.at(i)->getDiffuseStrength());
		}
	}

	// set shadow maps (point first, dir second and spot last)
	int nbDLights = m_directionalLights.size();
	int nbSLights = m_spotLights.size();

	int textureOffset{ 4 };
	int depthMapIndex{ 0 };
	
	for (int i{ 0 }; i < m_directionalLights.size(); ++i)
	{
		glm::vec3 lightPosition = m_directionalLights[i].GetPosition();
		glm::vec3 lightTarget = lightPosition + m_directionalLights[i].GetDirection();
		glm::mat4 lightView = glm::lookAt(lightPosition, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));

		glActiveTexture(GL_TEXTURE0 + textureOffset);
		glBindTexture(GL_TEXTURE_2D, m_stdDepth[depthMapIndex]->GetAttachments().at(0).id);
		SetUniform(GetUniformLocation(m_blinnPhong, "depthMap[" + std::to_string(depthMapIndex) + "]"), textureOffset);
		SetUniform(GetUniformLocation(m_blinnPhong, "light[" + std::to_string(i) + "].lightSpaceMatrix"), m_orthoProjection * lightView);
		depthMapIndex++;
		textureOffset++;
	}

	for (int i{ 0 }; i < m_spotLights.size(); ++i)
	{
		float outerCutOff = m_spotLights[i].GetOuterCutOff();
		glm::vec3 lightPosition = m_spotLights[i].GetPosition();
		glm::vec3 lightTarget = lightPosition + m_spotLights[i].GetDirection();
		glm::mat4 lightView = glm::lookAt(lightPosition, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 spotProj = glm::perspective(
			outerCutOff * 2.0f, 1.0f,
			0.01f,
			1000.0f
		);

		glActiveTexture(GL_TEXTURE0 + textureOffset);
		glBindTexture(GL_TEXTURE_2D, m_stdDepth[depthMapIndex]->GetAttachments().at(0).id);
		SetUniform(GetUniformLocation(m_blinnPhong, "depthMap[" + std::to_string(depthMapIndex) + "]"), textureOffset);
		SetUniform(GetUniformLocation(m_blinnPhong, "light[" + std::to_string(i + nbDLights) + "].lightSpaceMatrix"), spotProj * lightView);
		depthMapIndex++;
		textureOffset++;
	}

	drawScene(drawScenePass::BlinnPhong);

	if (m_gridAxis) m_gridAxis->Draw(m_perspective, m_camera->GetViewMatrix());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Scene::drawScene(drawScenePass scenePass)
{
	GLuint shader = 0;
	int modelMatrixId = -1;
	int normalMatrixId = -1;
	if (scenePass == drawScenePass::ShadowMapping)
	{
		shader = m_shadowMapping;
		modelMatrixId = m_shadowMappingShaderModelMatrixId;
	}
	else if (scenePass == drawScenePass::BlinnPhong)
	{
		shader = m_blinnPhong;
		modelMatrixId = m_blinnPhongShaderModelMatrixId;
		normalMatrixId = m_blinnPhongShaderNormalMatrixId;
	}
	else if (scenePass == drawScenePass::GBuffer)
	{
		modelMatrixId = glGetUniformLocation(m_outputGeoShader, "modelMatrix");
	}

	ModelDrawInfo drawInfo;
	drawInfo.bindMaterials = true;
	drawInfo.mode = GL_TRIANGLES;
	for (size_t i = 0; i < m_maxEnts; i++)
	{
		if (scenePass == drawScenePass::BlinnPhong)
		{
			drawInfo.shaderProgram = m_blinnPhong;
			SetUniform(GetUniformLocation(m_blinnPhong, "solid"), 1);
		}
		
		if (modelMatrixId >= 0)  SetUniform(modelMatrixId, m_entities[i]->modelMat);
		if (normalMatrixId >= 0) SetUniform(normalMatrixId, glm::mat3(glm::transpose(glm::inverse(m_entities[i]->modelMat))));
		m_entities[i]->model.Draw(drawInfo);
	}
}
//=============================================================================