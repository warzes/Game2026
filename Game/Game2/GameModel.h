#pragma once

#include "SceneObject.h"

enum class BlendingType : uint8_t
{
	Normal,
	Additive
};

enum class FaceVisibility : uint8_t
{
	Front,
	Back,
	Double
};

struct GameModelData final
{
	Model          model;

	glm::vec3      diffuseColor{ 1.0f };
	float          specularity{ 1.0f };
	float          shininess{ 10.0f };
	float          opacity{ 1.0f };
	float          tileU{ 1.0f };
	float          tileV{ 1.0f };
	
	bool           visible{ true };
	bool           castShadows{ true };
	bool           receiveShadows{ true };
	bool           isInstancedModel{ false };

	bool           alphaTest{ false };
	bool           transparency{ false };
	BlendingType   blendingType{ BlendingType::Normal };
	FaceVisibility faceVisibility{ FaceVisibility::Front };
};

/*
* надо переделать логику - модель должна быть обща€, а тут должен быть экземпл€р модели. модель грузитс€ один раз, а экземпл€ров указывающих на нее много. и можно реализовать инстансом
*/

class GameModel : public SceneObject
{
public:
	GameModel() : SceneObject(ObjectType::Model) {}

	bool LoadModel(const std::string& fileName);

	void SetupParameters(ProgramHandle program);

	GameModelData& GetData() { return m_data; }

private:
	GameModelData m_data;
};