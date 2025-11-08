#pragma once

#include "NanoRender.h"
#include "NanoScene.h"
#include "RP_1_DirectionalLightsShadowMap.h"
#include "RP_2_MainScene.h"
#include "RPGeometry.h"
#include "RPSSAO.h"
#include "RPSSAOBlur.h"
#include "RPBlinnPhong.h"
#include "RPComposite.h"
#include "GameWorldData.h"

struct GameObject final
{
	const AABB& GetAABB() const noexcept { return model.GetAABB(); }

	Model     model;
	glm::mat4 modelMat{ glm::mat4(1.0f) };
	bool      visible{ true };
};

class GameScene final
{
public:
	bool Init();
	void Close();
	void Draw();

	void BindCamera(Camera* camera);
	void BindGameObject(GameObject* go);
	void BindLight(DirectionalLight* ent);
	void BindLight(SpotLight* ent);
	void BindLight(PointLight* ent);

private:
	void beginDraw();
	void draw();
	void endDraw();

	void blittingToScreen(GLuint fbo, uint16_t srcWidth, uint16_t srcHeight);

	GameWorldData                m_data;

	RPDirectionalLightsShadowMap m_rpDirShadowMap;
	RPMainScene                  m_rpMainScene;

	RPGeometry                   m_rpGeometry;
	RPSSAO                       m_rpSSAO;
	RPSSAOBlur                   m_rpSSAOBlur;
	RPBlinnPhong                 m_rpBlinnPhong;

	RPComposite                  m_rpComposite;
};