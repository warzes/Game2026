#pragma once

#include "Framebuffer.h"
#include "NanoScene.h"

constexpr size_t MaxDirectionalLight = 4;

enum class ShadowQuality 
{
	Off = 0,
	Tiny = 256,
	Small = 512,
	Med = 1024,
	High = 2048,
	Ultra = 4096,
	Mega = 8192
};

struct Entity;

class RPDirShadowMap final
{
public:
	bool Init();
	void Close();

	void Draw(
		const std::vector<DirectionalLight*>& dirLights, size_t numDirLights,
		const std::vector<Entity*>& entites, size_t numEntities);

	void SetShadowQuality(ShadowQuality quality);

private:
	void drawScene(const std::vector<Entity*>& entites, size_t numEntities);
	GLuint        m_program{ 0 };
	int           m_projectionMatrixId{ -1 };
	int           m_viewMatrixId{ -1 };
	int           m_modelMatrixId{ -1 };
	//int           m_hasDiffuseId{ -1 };

	ShadowQuality m_shadowQuality;
	float         m_orthoDimension;
	glm::mat4     m_orthoProjection; // for directional lights

	std::array<std::unique_ptr<Framebuffer>, MaxDirectionalLight> m_depthFBO;
};