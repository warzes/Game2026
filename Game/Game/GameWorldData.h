#pragma once

class Camera;
struct OldGameObject;
struct DirectionalLight;
struct PointLight;
struct SpotLight;
struct AmbientBoxLight;
struct AmbientSphereLight;

struct GameWorldData final // TODO;
{
	void Init()
	{
		oldGameObjects.reserve(10000);
		dirLights.resize(MaxDirectionalLight);
		spotLights.resize(MaxSpotLight);
		pointLights.resize(MaxPointLight);
		boxLights.resize(MaxAmbientBoxLight);
		sphereLights.resize(MaxAmbientSphereLight);
	}

	void ResetFrame()
	{
		oldCamera = nullptr;
		numOldGameObject = 0;
		numDirLights = 0;
		numSpotLights = 0;
		numPointLights = 0;
		numBoxLights = 0;
		numSphereLights = 0;
	}

	Camera*                          oldCamera{ nullptr };
	std::vector<OldGameObject*>         oldGameObjects;
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