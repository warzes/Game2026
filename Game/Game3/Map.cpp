#include "stdafx.h"
#include "Map.h"
#include "TileMap.h"
#define MAPCHUNKSIZE 30
TileInfo tempMap[MAPCHUNKSIZE][MAPCHUNKSIZE][MAPCHUNKSIZE];
//=============================================================================
size_t addMeshInfo(std::vector<MeshInfo>& meshInfo, Texture2D texId);
//=============================================================================
void AddBox(
	const glm::vec3& center, float width, float height, float depth, const glm::vec3& color, std::vector<MeshVertex>& verticesWall, std::vector<unsigned int>& indicesWall, std::vector<MeshVertex>& verticesCeil, std::vector<unsigned int>& indicesCeil, std::vector<MeshVertex>& verticesFloor, std::vector<unsigned int>& indicesFloor,
	bool enablePlane[6]);
//=============================================================================
bool MapChunk::Init()
{
	for (size_t y = 0; y < MAPCHUNKSIZE; y++)
	{
		for (size_t x = 0; x < MAPCHUNKSIZE; x++)
		{
			for (size_t z = 0; z < MAPCHUNKSIZE; z++)
			{
				tempMap[x][y][z].type = TileGeometryType::None;
			}
			tempMap[x][y][0].type = TileGeometryType::FullBox1;
			tempMap[x][y][0].textureWall = textures::LoadTexture2D("data/tiles/grass01_wall.png", ColorSpace::Linear, true);
			tempMap[x][y][0].textureCeil = textures::LoadTexture2D("data/tiles/grass01_ceil.png");
			tempMap[x][y][0].textureFloor = textures::LoadTexture2D("data/tiles/grass01.png");
		}
	}
	tempMap[14][14][0].type = TileGeometryType::None;
	tempMap[14][15][0].type = TileGeometryType::None;
	tempMap[14][16][0].type = TileGeometryType::None;

	tempMap[15][14][0].type = TileGeometryType::None;
	tempMap[15][16][0].type = TileGeometryType::None;

	tempMap[16][14][0].type = TileGeometryType::None;
	tempMap[16][15][0].type = TileGeometryType::None;


	tempMap[15][15][0].type = TileGeometryType::FullBox2;
	tempMap[15][15][0].textureWall = textures::LoadTexture2D("data/tiles/grass01_wall.png", ColorSpace::Linear, true);
	tempMap[15][15][0].textureCeil = textures::LoadTexture2D("data/tiles/grass01_ceil.png");
	tempMap[15][15][0].textureFloor = textures::LoadTexture2D("data/tiles/grass01.png");

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
	std::vector<MeshInfo> meshInfo;

	const float mapOffset = MAPCHUNKSIZE / 2;
	for (size_t iy = 0; iy < MAPCHUNKSIZE; iy++)
	{
		for (size_t ix = 0; ix < MAPCHUNKSIZE; ix++)
		{
			for (size_t iz = 0; iz < MAPCHUNKSIZE; iz++)
			{
				auto id = tempMap[ix][iy][iz];
				if (id.type == TileGeometryType::None) continue;

				float x = float(ix) - mapOffset;
				float y = float(iy) - mapOffset;
				float z = float(iz);

				bool enablePlane[6] = { true }; // TODO: убрать невидимое

				size_t idWall = addMeshInfo(meshInfo, id.textureWall);
				size_t idFloor = addMeshInfo(meshInfo, id.textureFloor);
				size_t idCeil = addMeshInfo(meshInfo, id.textureCeil);

				float heightBlock = 1.0f;

				if (id.type == TileGeometryType::FullBox1)
				{
					heightBlock = 1.0f;
				}
				else if (id.type == TileGeometryType::FullBox0_25)
				{
					heightBlock = 0.25f;
				}
				else if (id.type == TileGeometryType::FullBox0_50)
				{
					heightBlock = 0.50f;
				}
				else if (id.type == TileGeometryType::FullBox0_75)
				{
					heightBlock = 0.75f;
				}
				else if (id.type == TileGeometryType::FullBox1_25)
				{
					heightBlock = 1.25f;
				}
				else if (id.type == TileGeometryType::FullBox1_50)
				{
					heightBlock = 1.25f;
				}
				else if (id.type == TileGeometryType::FullBox1_50)
				{
					heightBlock = 1.50f;
				}
				else if (id.type == TileGeometryType::FullBox1_75)
				{
					heightBlock = 1.75f;
				}
				else if (id.type == TileGeometryType::FullBox2)
				{
					heightBlock = 2.0f;
				}

				glm::vec3 center = glm::vec3(x, z + heightBlock / 2.0f, y);
				AddBox(center, 1.0f, heightBlock, 1.0f, id.color,
					meshInfo[idWall].vertices, meshInfo[idWall].indices,
					meshInfo[idCeil].vertices, meshInfo[idCeil].indices,
					meshInfo[idFloor].vertices, meshInfo[idFloor].indices,
					enablePlane);
			}
		}
	}
	m_model.model.Create(meshInfo);
}
//=============================================================================