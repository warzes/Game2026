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
	if (!m_rpMainScene.Init())
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
	m_rpMainScene.Draw(m_entities, m_numEntities);
}
//=============================================================================
void GameScene::endDraw()
{
	m_camera = nullptr;
	m_numEntities = 0;
	m_numDirLights = 0;
}
//=============================================================================