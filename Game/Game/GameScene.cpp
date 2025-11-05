#include "stdafx.h"
#include "GameScene.h"
#include "NanoWindow.h"
#include "NanoIO.h"
#include "NanoLog.h"
//=============================================================================
bool GameScene::Init()
{
	m_entities.reserve(10000);
	m_dirLights.resize(MaxDirectionalLight);

	const auto wndWidth = window::GetWidth();
	const auto wndHeight = window::GetHeight();

	if (!m_rpDirShadowMap.Init())
		return false;
	if (!m_rpGeometry.Init(wndWidth, wndHeight))
		return false;
	if (!m_rpSSAO.Init(wndWidth, wndHeight))
		return false;
	if (!m_rpSSAOBlur.Init(wndWidth, wndHeight))
		return false;
	if (!m_rpBlinnPhong.Init(wndWidth, wndHeight))
		return false;
	if (!m_rpComposite.Init(wndWidth, wndHeight))
		return false;
		
	return true;
}
//=============================================================================
void GameScene::Close()
{
	m_rpBlinnPhong.Close();
	m_rpComposite.Close();
	m_rpSSAOBlur.Close();
	m_rpSSAO.Close();
	m_rpGeometry.Close();
	m_rpDirShadowMap.Close();
}
//=============================================================================
void GameScene::BindCamera(Camera* camera)
{
	m_camera = camera;
}
//=============================================================================
void GameScene::BindEntity(Entity* ent)
{
	if (m_numEntities >= m_entities.size())
		m_entities.push_back(ent);
	else
		m_entities[m_numEntities] = ent;

	m_numEntities++;
}
//=============================================================================
void GameScene::BindLight(DirectionalLight* ent)
{
	if (m_numDirLights >= MaxDirectionalLight)
	{
		Error("Max dir light");
		return;
	}
	m_dirLights[m_numDirLights] = ent;
	m_numDirLights++;
}
//=============================================================================
void GameScene::Draw()
{
	if (!m_camera)
	{
		Warning("Not active camera");
		return;
	}
	if (!m_numEntities)
	{
		Warning("Not active entities");
		return;
	}

	beginDraw();
	draw();
	endDraw();
}
//=============================================================================
void GameScene::SetShadowQuality(ShadowQuality quality)
{
	m_rpDirShadowMap.SetShadowQuality(quality);
}
//=============================================================================
void GameScene::beginDraw()
{
	const auto wndWidth = window::GetWidth();
	const auto wndHeight = window::GetHeight();

	m_rpGeometry.Resize(wndWidth, wndHeight);
	m_rpSSAO.Resize(wndWidth, wndHeight);
	m_rpSSAOBlur.Resize(wndWidth, wndHeight);
	m_rpBlinnPhong.Resize(wndWidth, wndHeight);
	m_rpComposite.Resize(wndWidth, wndHeight);
}
//=============================================================================
void GameScene::draw()
{
	//================================================================================
	// 1 Render Pass: draw shadow maps
	//		Set state: glEnable(GL_DEPTH_TEST);
	m_rpDirShadowMap.Draw(m_dirLights, m_numDirLights, m_entities, m_numEntities);

	if (EnableSSAO)
	{
		//================================================================================
		// 2 Render Pass: geometry
		// TODO: m_rpGeometry можно не рендерить если отключено SSAO
		m_rpGeometry.Draw(m_entities, m_numEntities, m_camera);

		//================================================================================
		// 3 Render Pass: SSAO
		//		Set state: glDisable(GL_DEPTH_TEST);
		// TODO: m_rpSSAO можно не рендерить если отключено SSAO
		m_rpSSAO.Draw(m_rpGeometry.GetFBO());

		//================================================================================
		// 4 Render Pass: SSAO Blur
		// TODO: m_rpSSAO можно не рендерить если отключено SSAO
		m_rpSSAOBlur.Draw(m_rpSSAO.GetFBO());
	}
	
	//================================================================================
	// 5 Render Pass: main scenes
	//		Set state: glEnable(GL_DEPTH_TEST);
	m_rpBlinnPhong.Draw(m_rpDirShadowMap, m_dirLights, m_numDirLights, m_entities, m_numEntities, m_camera);
	
	//================================================================================
	// 6 Render Pass: post frame
	//		Set state: glDisable(GL_DEPTH_TEST);
	// TODO: m_rpSSAO можно не рендерить если отключено SSAO
	m_rpComposite.Draw(m_rpBlinnPhong.GetFBO(), EnableSSAO ? m_rpSSAOBlur.GetFBO() : nullptr);

	//================================================================================
	// 7 Render Pass: blitting main fbo
	blittingToScreen(m_rpComposite.GetFBOId(), m_rpComposite.GetWidth(), m_rpComposite.GetHeight());
}
//=============================================================================
void GameScene::endDraw()
{
	m_camera = nullptr;
	m_numEntities = 0;
	m_numDirLights = 0;
}
//=============================================================================
void GameScene::blittingToScreen(GLuint fbo, uint16_t srcWidth, uint16_t srcHeight)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(
		0, 0, srcWidth, srcHeight,
		0, 0, window::GetWidth(), window::GetHeight(),
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
//=============================================================================