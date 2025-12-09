#include "stdafx.h"
#include "GameScene.h"
//=============================================================================
bool GameScene::Init()
{
	m_data.Init();

	const auto wndWidth = window::GetWidth();
	const auto wndHeight = window::GetHeight();

	if (!m_shadowMap.Init(ShadowQuality::High))
		return false;
	if (!m_rpMainScene.Init(wndWidth, wndHeight))
		return false;


	if (!m_rpDirShadowMap.Init(ShadowQuality::High))
		return false;
	if (!m_oldrpMainScene.Init(wndWidth, wndHeight))
		return false;

	if (!m_rpComposite.Init(wndWidth * ScaleScreen, wndHeight * ScaleScreen))
		return false;

	return true;
}
//=============================================================================
void GameScene::Close()
{
	m_rpComposite.Close();
	m_oldrpMainScene.Close();
	m_rpDirShadowMap.Close();

	m_rpMainScene.Close();
	m_shadowMap.Close();
}
//=============================================================================
void GameScene::Bind(GameCamera* camera)
{
	m_data.activeCamera = camera;
}
//=============================================================================
void GameScene::Bind(GameModel* go)
{
	m_data.Bind(go);
}
//=============================================================================
void GameScene::Bind(GameDirectionalLight* go)
{
	m_data.Bind(go);
}
//=============================================================================
void GameScene::Bind(GamePointLight* go)
{
	m_data.Bind(go);
}
//=============================================================================
void GameScene::BindCamera(Camera* camera)
{
	m_data.oldCamera = camera;
}
//=============================================================================
void GameScene::BindGameObject(OldGameObject* go)
{
	if (m_data.numOldGameObject >= m_data.oldGameObjects.size())
		m_data.oldGameObjects.push_back(go);
	else
		m_data.oldGameObjects[m_data.numOldGameObject] = go;

	m_data.numOldGameObject++;
}
//=============================================================================
void GameScene::BindLight(DirectionalLight* ent)
{
	if (m_data.numDirLights >= MaxDirectionalLight)
	{
		Error("Max dir light");
		return;
	}
	m_data.dirLights[m_data.numDirLights] = ent;
	m_data.numDirLights++;
}
//=============================================================================
void GameScene::BindLight(SpotLight* ent)
{
	if (m_data.numSpotLights >= MaxSpotLight)
	{
		Error("Max Spot light");
		return;
	}
	m_data.spotLights[m_data.numSpotLights] = ent;
	m_data.numSpotLights++;
}
//=============================================================================
void GameScene::BindLight(PointLight* ent)
{
	if (m_data.numPointLights >= MaxPointLight)
	{
		Error("Max point light");
		return;
	}
	m_data.pointLights[m_data.numPointLights] = ent;
	m_data.numPointLights++;
}
//=============================================================================
void GameScene::BindLight(AmbientBoxLight* ent)
{
	if (m_data.numBoxLights >= MaxAmbientBoxLight)
	{
		Error("Max box light");
		return;
	}
	m_data.boxLights[m_data.numBoxLights] = ent;
	m_data.numBoxLights++;
}
//=============================================================================
void GameScene::BindLight(AmbientSphereLight* ent)
{
	if (m_data.numSphereLights >= MaxAmbientSphereLight)
	{
		Error("Max sphere light");
		return;
	}
	m_data.sphereLights[m_data.numSphereLights] = ent;
	m_data.numSphereLights++;
}
//=============================================================================
void GameScene::Draw()
{
	beginDraw();
	draw();
	endDraw();
}
//=============================================================================
void GameScene::beginDraw()
{
	const auto wndWidth = window::GetWidth();
	const auto wndHeight = window::GetHeight();

	m_rpMainScene.Resize(wndWidth, wndHeight);
	m_oldrpMainScene.Resize(wndWidth, wndHeight);
	m_rpComposite.Resize(wndWidth * ScaleScreen, wndHeight * ScaleScreen);
}
//=============================================================================
void GameScene::draw()
{
	if (!m_data.oldCamera || !m_data.countGameModels)
	{
		glClearColor(0.1f, 0.3f, 0.7f, 1.0f);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return;
	}

	// Shadow mapping pass
	if (m_data.countGameDirectionalLights > 0 || m_data.countGamePointLights > 0)
	{
		m_shadowMap.RenderShadows(m_data);
	}

	// Render scene
	m_rpMainScene.Draw(m_shadowMap, m_data);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return;





	//================================================================================
	// 1.) Render Pass: render depth of scene to texture (from light's perspective)
	m_rpDirShadowMap.Draw(m_data);
	//m_rpSpotShadowMap.Draw(m_data);
	//m_rpPointShadowMap.Draw(m_data);

	//================================================================================
	// 2.) Render Pass: render Scene as normal using the generated depth / shadow map
	m_oldrpMainScene.Draw(m_rpDirShadowMap, m_data);

	//================================================================================
	// 3.) Render Pass: SSAO
	if (EnableSSAO)
	{
		//================================================================================
		// 3.1 Render Pass: geometry
		//m_rpGeometry.Draw(m_data.gameObjects, m_data.numGameObject, m_data.camera);

		//================================================================================
		// 3.2 Render Pass: SSAO
		//		Set state: glDisable(GL_DEPTH_TEST);
		//m_rpSSAO.Draw(&m_rpGeometry.GetFBO());

		//================================================================================
		// 3.3 Render Pass: SSAO Blur
		//m_rpSSAOBlur.Draw(&m_rpSSAO.GetFBO());
	}

	//================================================================================
	// 4 Render Pass: post frame
	//		Set state: glDisable(GL_DEPTH_TEST);
	//m_rpComposite.Draw(&m_rpBlinnPhong.GetFBO(), EnableSSAO ? &m_rpSSAOBlur.GetFBO() : nullptr);
	m_rpComposite.Draw(&m_oldrpMainScene.GetFBO(), /*EnableSSAO ? &m_rpSSAOBlur.GetFBO() :*/ nullptr);

	//================================================================================
	// 5 Render Pass: blitting main fbo
	blittingToScreen(m_rpComposite.GetFBOId(), m_rpComposite.GetWidth(), m_rpComposite.GetHeight());
}
//=============================================================================
void GameScene::endDraw()
{
	m_data.ResetFrame();
}
//=============================================================================
void GameScene::blittingToScreen(GLuint fbo, uint16_t srcWidth, uint16_t srcHeight)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlitFramebuffer(
		0, 0, srcWidth, srcHeight,
		0, 0, window::GetWidth(), window::GetHeight(),
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
//=============================================================================
