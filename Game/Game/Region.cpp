#include "stdafx.h"
#include "Region.h"
Region gRegion;
//=============================================================================
MeshCreateInfo CreateHorizontalQuad(float width = 1.0f, float height = 1.0f)
{
	MeshCreateInfo meshCI{};


	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;

	// 4 вершины прямоугольника (четырехугольник в XZ-плоскости, Y = 0)
	meshCI.vertices = {
		// Вершина 0 - левый нижний
		{
		.position = glm::vec3(-halfWidth, 0.0f, -halfHeight),  // позиция
		.color = glm::vec3(1.0f, 1.0f, 1.0f),                 // цвет
		.normal = glm::vec3(0.0f, 1.0f, 0.0f),                // нормаль вверх
		.texCoord = glm::vec2(0.0f, 0.0f),                    // текстурные координаты
		.tangent = glm::vec3(1.0f, 0.0f, 0.0f),               // тангент
		.bitangent = glm::vec3(0.0f, 0.0f, 1.0f)              // битангент
		},
		// Вершина 1 - правый нижний
		{
		.position = glm::vec3(halfWidth, 0.0f, -halfHeight),
		.color = glm::vec3(1.0f, 1.0f, 1.0f),
		.normal = glm::vec3(0.0f, 1.0f, 0.0f),
		.texCoord = glm::vec2(1.0f, 0.0f),
		.tangent = glm::vec3(1.0f, 0.0f, 0.0f),
		.bitangent = glm::vec3(0.0f, 0.0f, 1.0f)
		},
		// Вершина 2 - правый верхний
		{
		.position = glm::vec3(halfWidth, 0.0f, halfHeight),
		.color = glm::vec3(1.0f, 1.0f, 1.0f),
		.normal = glm::vec3(0.0f, 1.0f, 0.0f),
		.texCoord = glm::vec2(1.0f, 1.0f),
		.tangent = glm::vec3(1.0f, 0.0f, 0.0f),
		.bitangent = glm::vec3(0.0f, 0.0f, 1.0f)
		},
		// Вершина 3 - левый верхний
		{
		.position = glm::vec3(-halfWidth, 0.0f, halfHeight),
		.color = glm::vec3(1.0f, 1.0f, 1.0f),
		.normal = glm::vec3(0.0f, 1.0f, 0.0f),
		.texCoord = glm::vec2(0.0f, 1.0f),
		.tangent = glm::vec3(1.0f, 0.0f, 0.0f),
		.bitangent = glm::vec3(0.0f, 0.0f, 1.0f)
		}
	};

	// Индексы для отрисовки двух треугольников
	meshCI.indices = {
	0, 1, 2,  // первый треугольник
	2, 3, 0   // второй треугольник
	};

	meshCI.material = Material{};
	meshCI.material->diffuseColor = glm::vec3(1.0f, 0.8f, 0.4f);

	return meshCI;
}
//=============================================================================
void Region::Init()
{
	std::vector<MeshCreateInfo> meshCI(1);

	meshCI[0] = CreateHorizontalQuad();

	model.model.Create(meshCI);
}
//=============================================================================
void Region::Close()
{
	model.model.Free();
}
//=============================================================================