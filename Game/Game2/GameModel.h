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
	glm::mat4      modelMat{ glm::mat4(1.0f) };

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

class GameModel : public SceneObject
{
public:
	bool LoadModel(const std::string& fileName);
	void SetWorldTransform(const glm::mat4& mat) { m_data.modelMat = mat; }

	void SetupParameters(ProgramHandle program);

	GameModelData& GetData() { return m_data; }

private:
	GameModelData m_data;
};