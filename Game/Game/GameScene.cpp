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
		
	return true;
}
//=============================================================================
void GameScene::Close()
{
	m_rpDirShadowMap.Close();
	m_rpMainScene.Close();
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
		Error("Max dir light 4");
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
}
//=============================================================================
void GameScene::draw()
{
	m_rpDirShadowMap.Draw(m_dirLights, m_numDirLights, m_entities, m_numEntities);
	m_rpMainScene.Draw(m_rpDirShadowMap, m_dirLights, m_numDirLights, m_entities, m_numEntities, m_camera);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rpMainScene.GetFBOId());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 0 = default framebuffer (экран)
	glBlitFramebuffer(
		0, 0, m_rpMainScene.GetWidth(), m_rpMainScene.GetHeight(), 
		0, 0, m_rpMainScene.GetWidth(), m_rpMainScene.GetHeight(),
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