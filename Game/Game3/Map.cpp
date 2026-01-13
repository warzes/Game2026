#include "stdafx.h"
#include "Map.h"
#include "TileMap.h"
#include "MapLoadObjTile.h"
#define MAPCHUNKSIZE 30
size_t tempMap[MAPCHUNKSIZE][MAPCHUNKSIZE][MAPCHUNKSIZE] = { 0 };
//=============================================================================
size_t addMeshInfo(std::vector<MeshInfo>& meshInfo, Texture2D texId)
{
	for (size_t i = 0; i < meshInfo.size(); i++)
	{
		if (meshInfo[i].material->diffuseTextures[0] == texId)
		{
			return i;
		}
	}

	MeshInfo nmi{};
	nmi.material = Material();
	nmi.material->diffuseTextures.push_back(texId);
	meshInfo.push_back(nmi);
	return meshInfo.size() - 1;
}
//=============================================================================
void optimizeMesh(std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices)
{
	// TODO:

	// не работает
	//// Шаг 1: Удаление дублирующихся вершин
	//std::vector<unsigned int> remap(vertices.size());
	//size_t uniqueVertices = meshopt_generateVertexRemap(remap.data(), indices.data(), indices.size(), vertices.data(), vertices.size(), sizeof(MeshVertex));

	//std::vector<MeshVertex> optimizedVertices(uniqueVertices);
	//meshopt_remapVertexBuffer(optimizedVertices.data(), vertices.data(), vertices.size(), sizeof(MeshVertex), remap.data());

	//meshopt_remapIndexBuffer(indices.data(), indices.data(), indices.size(), remap.data());

	//// Шаг 2: Удаление вырожденных и дублирующихся треугольников
	//std::vector<unsigned int> uniqueIndices;
	//std::set<std::array<unsigned int, 3>> triangleSet; // Используем set для уникальности треугольников

	//for (size_t i = 0; i < indices.size(); i += 3)
	//{
	//	std::array<unsigned int, 3> triangle = { indices[i], indices[i + 1], indices[i + 2] };
	//	std::sort(triangle.begin(), triangle.end()); // Сортируем вершины треугольника для уникальности (т.к. треугольник ABC == BCA == CAB)

	//	if (triangleSet.find(triangle) == triangleSet.end())
	//	{
	//		triangleSet.insert(triangle);
	//		uniqueIndices.push_back(indices[i]);
	//		uniqueIndices.push_back(indices[i + 1]);
	//		uniqueIndices.push_back(indices[i + 2]);
	//	}
	//}

	//indices = uniqueIndices;

	//vertices = optimizedVertices;
}
//=============================================================================
bool MapChunk::Init()
{
	TileInfo tempTile;
	tempTile.type = TileGeometryType::Block00;
	tempTile.textureWall = textures::LoadTexture2D("data/tiles/grass01_wall.png", ColorSpace::Linear, true);
	tempTile.textureCeil = textures::LoadTexture2D("data/tiles/grass01_ceil.png");
	tempTile.textureFloor = textures::LoadTexture2D("data/tiles/grass01.png");

	for (size_t y = 0; y < MAPCHUNKSIZE; y++)
	{
		for (size_t x = 0; x < MAPCHUNKSIZE; x++)
		{
			for (size_t z = 0; z < MAPCHUNKSIZE; z++)
			{
				tempMap[x][y][z] = NoTile;
				
			}

			tempMap[x][y][0] = TileBank::AddTileInfo(tempTile);
		}
	}
	tempTile.type = TileGeometryType::None;

	tempMap[14][14][0] = NoTile;
	tempMap[14][15][0] = NoTile;
	tempMap[14][16][0] = NoTile;
	tempMap[15][14][0] = NoTile;
	tempMap[15][16][0] = NoTile;
	tempMap[16][14][0] = NoTile;
	tempMap[16][15][0] = NoTile;



	tempTile.type = TileGeometryType::Block01;
	tempTile.textureWall = textures::LoadTexture2D("data/tiles/grass01_wall.png", ColorSpace::Linear, true);
	tempTile.textureCeil = textures::LoadTexture2D("data/tiles/grass01_ceil.png");
	tempTile.textureFloor = textures::LoadTexture2D("data/tiles/grass01.png");
	tempTile.rotate = RotateAngleY::Rotate270;
	tempMap[16][17][1] = TileBank::AddTileInfo(tempTile);
	tempTile.rotate = RotateAngleY::Rotate0;
	tempMap[17][16][1] = TileBank::AddTileInfo(tempTile);
	tempTile.rotate = RotateAngleY::Rotate180;
	tempMap[17][18][1] = TileBank::AddTileInfo(tempTile);
	tempTile.rotate = RotateAngleY::Rotate90;
	tempMap[18][17][1] = TileBank::AddTileInfo(tempTile);

	tempTile.type = TileGeometryType::Block00;
	tempTile.rotate = RotateAngleY::Rotate0;
	tempMap[17][17][1] = TileBank::AddTileInfo(tempTile);

	//tempTile.type = TileGeometryType::NewBox2;
	//tempTile.textureWall = textures::LoadTexture2D("data/tiles/grass01_wall.png", ColorSpace::Linear, true);
	//tempTile.textureCeil = textures::LoadTexture2D("data/tiles/grass01_ceil.png");
	//tempTile.textureFloor = textures::LoadTexture2D("data/tiles/grass01.png");
	////tempTile.height = 1.5f;
	//tempMap[16][17][1] = TileBank::AddTileInfo(tempTile);

	//tempTile.type = TileGeometryType::NewBox;
	//tempTile.textureWall = textures::LoadTexture2D("data/tiles/grass01_wall.png", ColorSpace::Linear, true);
	//tempTile.textureCeil = textures::LoadTexture2D("data/tiles/grass01_ceil.png");
	//tempTile.textureFloor = textures::LoadTexture2D("data/tiles/grass01.png");
	////tempTile.height = 1.5f;
	//tempMap[18][17][1] = TileBank::AddTileInfo(tempTile);


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
				if (tempMap[ix][iy][iz] == NoTile) continue;

				auto id = *TileBank::GetTileInfo(tempMap[ix][iy][iz]);
				if (id.type == TileGeometryType::None) continue;

				float x = float(ix) - mapOffset;
				float y = float(iy) - mapOffset;
				float z = float(iz);

				size_t idWall = addMeshInfo(meshInfo, id.textureWall);
				size_t idFloor = addMeshInfo(meshInfo, id.textureFloor);
				size_t idCeil = addMeshInfo(meshInfo, id.textureCeil);

				glm::vec3 center = glm::vec3(x, z + 0.5f, y);

				BlockModelInfo blockModelInfo{};
				blockModelInfo.color = id.color;
				blockModelInfo.center = center;
				blockModelInfo.rotate = glm::vec3(0.0f);
				if (id.rotate == RotateAngleY::Rotate0)
					blockModelInfo.rotate.y = 0.0f;
				else if (id.rotate == RotateAngleY::Rotate90)
					blockModelInfo.rotate.y = glm::radians(90.0f);
				else if (id.rotate == RotateAngleY::Rotate180)
					blockModelInfo.rotate.y = glm::radians(180.0f);
				else if (id.rotate == RotateAngleY::Rotate270)
					blockModelInfo.rotate.y = glm::radians(270.0f);
				
				setVisibleBlock(id, blockModelInfo, ix, iy, iz);
				if (id.type == TileGeometryType::Block00)
				{
					blockModelInfo.modelPath = "data/tiles/Block00.obj";
				}
				else if (id.type == TileGeometryType::Block01)
				{
					blockModelInfo.modelPath = "data/tiles/Block01.obj";
				}
				AddObjModel(blockModelInfo,
					meshInfo[idWall].vertices, meshInfo[idWall].indices,
					meshInfo[idCeil].vertices, meshInfo[idCeil].indices,
					meshInfo[idFloor].vertices, meshInfo[idFloor].indices);
			}
		}
	}

	for (size_t i = 0; i < meshInfo.size(); i++)
	{
		optimizeMesh(meshInfo[i].vertices, meshInfo[i].indices);

		m_vertCount += meshInfo[i].vertices.size();
		m_indexCount += meshInfo[i].indices.size();
	}

	m_model.model.Create(meshInfo);
}
//=============================================================================
bool testVisBlock(size_t x, size_t y, size_t z)
{
	if ((x >= MAPCHUNKSIZE) || (y >= MAPCHUNKSIZE) || (z >= MAPCHUNKSIZE)) return false;
	if (tempMap[x][y][z] == NoTile) return false;

	const auto& b = *TileBank::GetTileInfo(tempMap[x][y][z]);
	if (b.type == TileGeometryType::Block00)
	{
		return true;
	}
	else
	{
		// TODO: другие варианты блоков
	}
	return false;
}
//=============================================================================
void MapChunk::setVisibleBlock(const TileInfo& ti, BlockModelInfo& blockModelInfo, size_t x, size_t y, size_t z)
{
	if (ti.type == TileGeometryType::Block00)
	{
		if (x > 0) blockModelInfo.rightVisible = !testVisBlock(x - 1, y, z);
		if (x < MAPCHUNKSIZE - 1) blockModelInfo.leftVisible = !testVisBlock(x + 1, y, z);

		if (y > 0) blockModelInfo.forwardVisible = !testVisBlock(x, y - 1, z);
		if (y < MAPCHUNKSIZE - 1) blockModelInfo.backVisible = !testVisBlock(x, y + 1, z);

		if (z > 0) blockModelInfo.bottomVisible = !testVisBlock(x, y, z - 1);
		if (z < MAPCHUNKSIZE - 1) blockModelInfo.topVisible = !testVisBlock(x, y, z + 1);
	}
	else
	{
		// TODO: другие варианты блоков
	}
}
//=============================================================================