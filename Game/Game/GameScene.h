#pragma once

#include "NanoRender.h"
#include "NanoScene.h"
#include "GameWorldData.h"
#include "RenderPass1.h"

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

	GameWorldData m_data;
	RenderPass1   m_rpDirShadowMap;
};