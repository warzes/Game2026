#include "stdafx.h"
#include "GameScene.h"
#include "NanoWindow.h"
#include "NanoIO.h"
#include "NanoLog.h"
//=============================================================================
bool GameScene::Init()
{
	m_data.Init();

	const auto wndWidth = window::GetWidth();
	const auto wndHeight = window::GetHeight();

	if (!m_rpDirShadowMap.Init(ShadowQuality::High))
		return false;



	return true;
}
//=============================================================================
void GameScene::Close()
{
	m_rpDirShadowMap.Close();
}
//=============================================================================
void GameScene::BindCamera(Camera* camera)
{
	m_data.camera = camera;
}
//=============================================================================
void GameScene::BindGameObject(GameObject* go)
{
	if (m_data.numGameObject >= m_data.gameObjects.size())
		m_data.gameObjects.push_back(go);
	else
		m_data.gameObjects[m_data.numGameObject] = go;

	m_data.numGameObject++;
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
void GameScene::Draw()
{
	if (!m_data.camera)
	{
		Warning("Not active camera");
		return;
	}
	if (!m_data.numGameObject)
	{
		Warning("Not active entities");
		return;
	}

	beginDraw();
	draw();
	endDraw();
}
//=============================================================================
void GameScene::beginDraw()
{
	const auto wndWidth = window::GetWidth();
	const auto wndHeight = window::GetHeight();
}
//=============================================================================
void GameScene::draw()
{
	//================================================================================
	// 1.) Render Pass: render depth of scene to texture (from light's perspective)
	m_rpDirShadowMap.Draw(m_data);
	//m_rpSpotShadowMap.Draw(m_data);
	//m_rpPointShadowMap.Draw(m_data);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void GameScene::endDraw()
{
	m_data.ResetFrame();
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
