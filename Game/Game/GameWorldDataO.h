#pragma once

class Camera;
struct GameObjectO;
struct DirectionalLight;
struct PointLight;
struct SpotLight;

struct GameWorldDataO final // TODO;
{
	void Init()
	{
		gameObjects.reserve(10000);
		dirLights.resize(MaxDirectionalLight);
		spotLights.resize(MaxSpotLight);
		pointLights.resize(MaxPointLight);
	}

	void ResetFrame()
	{
		camera = nullptr;
		numGameObject = 0;
		numDirLights = 0;
		numSpotLights = 0;
		numPointLights = 0;
	}

	Camera*                        camera{ nullptr };
	std::vector<GameObjectO*>       gameObjects;
	size_t                         numGameObject{ 0 };
	std::vector<DirectionalLight*> dirLights;
	size_t                         numDirLights{ 0 };
	std::vector<SpotLight*>        spotLights;
	size_t                         numSpotLights{ 0 };
	std::vector<PointLight*>       pointLights;
	size_t                         numPointLights{ 0 };
};