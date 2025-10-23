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

	if (!m_rpDirShadowMap.Init())
		return false;
	if (!m_rpMainScene.Init(window::GetWidth(), window::GetHeight()))
		return false;
	if (!m_rpPostFrame.Init(window::GetWidth(), window::GetHeight()))
		return false;
		
	return true;
}
//=============================================================================
void GameScene::Close()
{
	m_rpDirShadowMap.Close();
	m_rpMainScene.Close();
	m_rpPostFrame.Close();
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
	m_rpMainScene.Resize(window::GetWidth(), window::GetHeight());
	m_rpPostFrame.Resize(window::GetWidth(), window::GetHeight());
}
//=============================================================================
void GameScene::draw()
{
	//================================================================================
	// 1 Render Pass: draw shadow maps
	//		Set state: glEnable(GL_DEPTH_TEST);
	m_rpDirShadowMap.Draw(m_dirLights, m_numDirLights, m_entities, m_numEntities);
	
	//================================================================================
	// 2 Render Pass: main scenes
	m_rpMainScene.Draw(m_rpDirShadowMap, m_dirLights, m_numDirLights, m_entities, m_numEntities, m_camera);
	
	//================================================================================
	// 3 Render Pass: post frame
	//		Set state: glDisable(GL_DEPTH_TEST);
	m_rpPostFrame.Draw(m_rpMainScene.GetFBO());

	//================================================================================
	// 4 Render Pass: blitting main fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rpPostFrame.GetFBOId());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 0 = default framebuffer (экран)
	glBlitFramebuffer(
		0, 0, m_rpPostFrame.GetWidth(), m_rpPostFrame.GetHeight(),
		0, 0,       window::GetWidth(),       window::GetHeight(),
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
//=============================================================================
void GameScene::endDraw()
{
	m_camera = nullptr;
	m_numEntities = 0;
	m_numDirLights = 0;
}
//=============================================================================