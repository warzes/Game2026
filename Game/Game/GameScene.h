#pragma once

#include "NanoRender.h"
#include "NanoScene.h"
#include "RPDirShadowMap.h"
#include "RPMainScene.h"

struct Entity final
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
	void BindEntity(Entity* ent);
	void BindLight(DirectionalLight* ent);

	void SetShadowQuality(ShadowQuality quality);

private:
	void beginDraw();
	void draw();
	void endDraw();

	Camera*                        m_camera{ nullptr };
	std::vector<Entity*>           m_entities;
	size_t                         m_numEntities{ 0 };
	std::vector<DirectionalLight*> m_dirLights;
	size_t                         m_numDirLights{ 0 };

	RPDirShadowMap                 m_rpDirShadowMap;
	RPMainScene                    m_rpMainScene;
};