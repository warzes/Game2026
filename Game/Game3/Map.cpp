#include "stdafx.h"
#include "Map.h"
#define MAPCHUNKSIZE 10
int tempMap[MAPCHUNKSIZE][MAPCHUNKSIZE];
//=============================================================================
void AddCube(const glm::vec3& center, float size, std::vector<MeshVertex>& vertices, std::vector<unsigned int>& indices)
{
	float halfSize = size / 2.0f;

	// Define the 8 corner points relative to the center
	glm::vec3 p0 = center + glm::vec3(-halfSize, -halfSize, -halfSize); // 0: Left-Bottom-Front
	glm::vec3 p1 = center + glm::vec3(halfSize, -halfSize, -halfSize); // 1: Right-Bottom-Front
	glm::vec3 p2 = center + glm::vec3(halfSize, halfSize, -halfSize); // 2: Right-Top-Front
	glm::vec3 p3 = center + glm::vec3(-halfSize, halfSize, -halfSize); // 3: Left-Top-Front

	glm::vec3 p4 = center + glm::vec3(-halfSize, -halfSize, halfSize); // 4: Left-Bottom-Back
	glm::vec3 p5 = center + glm::vec3(halfSize, -halfSize, halfSize); // 5: Right-Bottom-Back
	glm::vec3 p6 = center + glm::vec3(halfSize, halfSize, halfSize); // 6: Right-Top-Back
	glm::vec3 p7 = center + glm::vec3(-halfSize, halfSize, halfSize); // 7: Left-Top-Back

	// Face Colors (optional, for visual distinction)
	glm::vec3 red(1.0f, 0.0f, 0.0f);
	glm::vec3 green(0.0f, 1.0f, 0.0f);
	glm::vec3 blue(0.0f, 0.0f, 1.0f);
	glm::vec3 yellow(1.0f, 1.0f, 0.0f);
	glm::vec3 cyan(0.0f, 1.0f, 1.0f);
	glm::vec3 magenta(1.0f, 0.0f, 1.0f);

	// Front Face Z = center.z - halfSize (p0, p1, p2, p3) - CCW order for outward facing normal
	GeometryGenerator::AddPlane(p0, p3, p2, p1, vertices, indices, blue, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0));

	// Back Face Z = center.z + halfSize (p5, p4, p7, p6) - CCW order for outward facing normal
	GeometryGenerator::AddPlane(p5, p6, p7, p4, vertices, indices, red, glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1));

	// Top Face Y = center.y + halfSize (p3, p2, p6, p7) - CCW order for outward facing normal
	GeometryGenerator::AddPlane(p3, p7, p6, p2, vertices, indices, green, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0));

	// Bottom Face Y = center.y - halfSize (p4, p5, p1, p0) - CCW order for outward facing normal
	GeometryGenerator::AddPlane(p4, p0, p1, p5, vertices, indices, yellow, glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1));

	// Right Face X = center.x + halfSize (p1, p5, p6, p2) - CCW order for outward facing normal
	GeometryGenerator::AddPlane(p1, p2, p6, p5, vertices, indices, cyan, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0));

	// Left Face X = center.x - halfSize (p4, p0, p3, p7) - CCW order for outward facing normal
	GeometryGenerator::AddPlane(p4, p7, p3, p0, vertices, indices, magenta, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0));
}
//=============================================================================
bool MapChunk::Init()
{
	for (size_t y = 0; y < MAPCHUNKSIZE; y++)
	{
		for (size_t x = 0; x < MAPCHUNKSIZE; x++)
		{
			tempMap[x][y] = 0;
		}
	}

	tempMap[5][6] = 1;
	tempMap[4][5] = 1;
	tempMap[5][5] = 1;
	tempMap[5][4] = 1;
	tempMap[6][5] = 1;

	generateBufferMap();

	return true;
}
//=============================================================================
void MapChunk::Close()
{
}
//=============================================================================
void MapChunk::generateBufferMap()
{
	std::vector<MeshInfo> meshInfo(10);
	meshInfo[0].material = Material();
	meshInfo[0].material->diffuseTextures.push_back(textures::LoadTexture2D("data/pics/wall1.png"));
	meshInfo[1].material = Material();
	meshInfo[1].material->diffuseTextures.push_back(textures::LoadTexture2D("data/pics/tile1.png"));

	const float mapOffset = MAPCHUNKSIZE / 2;
	for (size_t iy = 0; iy < MAPCHUNKSIZE; iy++)
	{
		for (size_t ix = 0; ix < MAPCHUNKSIZE; ix++)
		{
			size_t id = tempMap[ix][iy];
			if (id == 99) continue;

			float x = float(ix) - mapOffset;
			float y = float(iy) - mapOffset;

			AddCube(glm::vec3(x, 0.5f + rand() % 10/10.0f, y), 1.0f, meshInfo[id].vertices, meshInfo[id].indices);
		}
	}
	m_model.model.Create(meshInfo);
}
//=============================================================================