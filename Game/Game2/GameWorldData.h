#pragma once

class GameCamera;
class GameModel;
class GameDirectionalLight;
class GamePointLight;

class Camera;
struct OldGameObject;
struct DirectionalLight;
struct PointLight;
struct SpotLight;
struct AmbientBoxLight;
struct AmbientSphereLight;

struct GameWorldData final
{
	void Init()
	{
		oldGameObjects.reserve(10000);
		gameModels.reserve(10000);
		dirLights.resize(MaxDirectionalLight);
		spotLights.resize(MaxSpotLight);
		pointLights.resize(MaxPointLight);
		boxLights.resize(MaxAmbientBoxLight);
		sphereLights.resize(MaxAmbientSphereLight);
	}

	void ResetFrame()
	{
		activeCamera = nullptr;
		countGameModels = 0;
		countGameDirectionalLights = 0;
		countGamePointLights = 0;

		oldCamera = nullptr;
		numOldGameObject = 0;
		numDirLights = 0;
		numSpotLights = 0;
		numPointLights = 0;
		numBoxLights = 0;
		numSphereLights = 0;
	}

	void Bind(GameModel* go)
	{
		if (countGameModels >= gameModels.size())
			gameModels.push_back(go);
		else
			gameModels[countGameModels] = go;

		countGameModels++;
	}

	void Bind(GameDirectionalLight* go)
	{
		if (countGameDirectionalLights >= gameDirectionalLights.size())
			gameDirectionalLights.push_back(go);
		else
			gameDirectionalLights[countGameDirectionalLights] = go;

		countGameDirectionalLights++;
	}

	void Bind(GamePointLight* go)
	{
		if (countGamePointLights >= gamePointLights.size())
			gamePointLights.push_back(go);
		else
			gamePointLights[countGamePointLights] = go;

		countGamePointLights++;
	}


	GameCamera*                        activeCamera{ nullptr };
	std::vector<GameModel*>            gameModels;
	size_t                             countGameModels{ 0 };
	std::vector<GameDirectionalLight*> gameDirectionalLights;
	size_t                             countGameDirectionalLights{ 0 };
	std::vector<GamePointLight*>       gamePointLights;
	size_t                             countGamePointLights{ 0 };

	// old
	Camera*                          oldCamera{ nullptr };
	std::vector<OldGameObject*>      oldGameObjects;
	size_t                           numOldGameObject{ 0 };
	std::vector<DirectionalLight*>   dirLights;
	size_t                           numDirLights{ 0 };
	std::vector<SpotLight*>          spotLights;
	size_t                           numSpotLights{ 0 };
	std::vector<PointLight*>         pointLights;
	size_t                           numPointLights{ 0 };
	std::vector<AmbientBoxLight*>    boxLights;
	size_t                           numBoxLights{ 0 };
	std::vector<AmbientSphereLight*> sphereLights;
	size_t                           numSphereLights{ 0 };
};