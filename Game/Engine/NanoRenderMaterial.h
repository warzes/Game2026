#pragma once

#include "NanoRenderTextures.h"

struct Material;

struct MaterialShaderSlot final
{
	void Bind(GLuint program, const Material& material);

	int diffuseTextures{ -1 };
	int diffuseColor{ -1 };
};

// TODO: в движке не нужно материал. А только массив текстур и возможно значения цветов
struct Material final
{
	float     opacity{ 1.0f };
	glm::vec3 diffuseColor{ 1.0f };
	glm::vec3 specularColor{ 1.0f };
	glm::vec3 ambientColor{ 1.0f };
	float     shininess{ 64.f };
	float     roughness{ 0.0f };
	float     metallic{ 0.0f };

	std::vector<Texture2D> diffuseTextures;
	std::vector<Texture2D> specularTextures;
	std::vector<Texture2D> normalTextures;
	std::vector<Texture2D> shininessTextures;
	std::vector<Texture2D> emissionTextures;
	std::vector<Texture2D> opacityTextures;
	//std::vector<Texture2D> depthTextures;

	bool noLighing{ false };
};

struct PBRMaterial final
{
	Texture2D albedoTexture;
	Texture2D normalTexture;
	Texture2D metallicRoughnessTexture;
	Texture2D AOTexture;
	Texture2D emissiveTexture;
};

struct PBRMaterialShaderSlot final
{
	int albedoTexture{ -1 };
	int normalTexture{ -1 };
	int metallicRoughnessTexture{ -1 };
	int AOTexture{ -1 };
	int emissiveTexture{ -1 };

	int hasAlbedo{ -1 };
	int hasNormal{ -1 };
	int hasMetallicRoughness{ -1 };
	int hasAO{ -1 };
	int hasEmissive{ -1 };
};