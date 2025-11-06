#pragma once

#include "NanoRender.h"
#include "NanoScene.h"
#include "RPDirShadowMap.h"
#include "RPGeometry.h"
#include "RPSSAO.h"
#include "RPSSAOBlur.h"
#include "RPBlinnPhong.h"
#include "RPMainScene.h"
#include "RPComposite.h"

struct GameObject final
{
	const AABB& GetAABB() const noexcept { return model.GetAABB(); }

	Model     model;
	glm::mat4 modelMat{ glm::mat4(1.0f) };
	bool      visible{ true };
};

struct GameLight final
{
	glm::vec3 position;
	glm::vec3 color;
	bool active{ false };
};

class GameScene final
{
public:
	bool Init();
	void Close();
	void Draw();

	void BindCamera(Camera* camera);
	void BindGameObject(GameObject* go);
	void BindLight(GameLight* ent);
	void BindLight(DirectionalLight* ent);

	void SetShadowQuality(ShadowQuality quality);

private:
	void beginDraw();
	void draw();
	void endDraw();

	void blittingToScreen(GLuint fbo, uint16_t srcWidth, uint16_t srcHeight);

	Camera*                        m_camera{ nullptr };
	std::vector<GameObject*>       m_gameObjects;
	size_t                         m_numGO{ 0 };
	std::vector<DirectionalLight*> m_dirLights;
	size_t                         m_numDirLights{ 0 };

	std::vector<GameLight*>        m_lights;
	size_t                         m_numLights{ 0 };

	RPDirShadowMap                 m_rpDirShadowMap;
	RPGeometry                     m_rpGeometry;
	RPSSAO                         m_rpSSAO;
	RPSSAOBlur                     m_rpSSAOBlur;
	RPBlinnPhong                   m_rpBlinnPhong;
	RPMainScene                    m_rpMainScene;
	RPComposite                    m_rpComposite;
};