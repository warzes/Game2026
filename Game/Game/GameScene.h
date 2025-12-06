#pragma once

#include "NanoRender.h"
#include "NanoScene.h"
#include "GameWorldData.h"
#include "RenderPass1.h"
#include "RenderPass2.h"
#include "RenderPass6.h"

struct OldGameObject final
{
	const AABB& GetAABB() const noexcept { return model.GetAABB(); }

	Model     model;
	glm::mat4 modelMat{ glm::mat4(1.0f) };
	bool      visible{ true };
};

class OldGameScene final
{
public:
	bool Init();
	void Close();
	void Draw();

	void BindCamera(Camera* camera);
	void BindGameObject(OldGameObject* go);
	void BindLight(DirectionalLight* ent);
	void BindLight(SpotLight* ent);
	void BindLight(PointLight* ent);
	void BindLight(AmbientBoxLight* ent);
	void BindLight(AmbientSphereLight* ent);

private:
	void beginDraw();
	void draw();
	void endDraw();

	void blittingToScreen(GLuint fbo, uint16_t srcWidth, uint16_t srcHeight);

	GameWorldData m_data;
	RenderPass1   m_rpDirShadowMap;
	RenderPass2   m_rpMainScene;
	// SSAO
	//RenderPass3   m_rpGeometry;
	//RenderPass4   m_rpSSAO;
	//RenderPass5   m_rpSSAOBlur;

	RenderPass6   m_rpComposite;
};