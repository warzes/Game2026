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
inline std::string getFileNameBlock(TileGeometryType type)
{
	switch (type)
	{
	case TileGeometryType::Block00: return "data/tiles/Block00.obj";
	case TileGeometryType::Block01: return "data/tiles/Block01.obj";
	case TileGeometryType::Block02: return "data/tiles/Block02.obj";
	case TileGeometryType::Block03: return "data/tiles/Block03.obj";
	case TileGeometryType::Block04: return "data/tiles/Block04.obj";
	case TileGeometryType::Block05: return "data/tiles/Block05.obj";
	case TileGeometryType::Block06: return "data/tiles/Block06.obj";
	case TileGeometryType::Block07: return "data/tiles/Block07.obj";
	case TileGeometryType::Block08: return "data/tiles/Block08.obj";
	case TileGeometryType::Block09: return "data/tiles/Block09.obj";
	case TileGeometryType::Block10: return "data/tiles/Block10.obj";
	default: std::unreachable();
	}
}
//=============================================================================
inline glm::vec3 getRotateAngle(RotateAngleY angle)
{
	glm::vec3 r(0.0f);
	if (angle == RotateAngleY::Rotate0)        r.y = 0.0f;
	else if (angle == RotateAngleY::Rotate90)  r.y = glm::radians(90.0f);
	else if (angle == RotateAngleY::Rotate180) r.y = glm::radians(180.0f);
	else if (angle == RotateAngleY::Rotate270) r.y = glm::radians(270.0f);
	return r;
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

	const float mapOffset = MAPCHUNKSIZE / 2.0f;
	for (size_t iy = 0; iy < MAPCHUNKSIZE; iy++)
	{
		for (size_t ix = 0; ix < MAPCHUNKSIZE; ix++)
		{
			for (size_t iz = 0; iz < MAPCHUNKSIZE; iz++)
			{
				if (tempMap[ix][iy][iz] == NoTile) continue;

				const auto& id = TileBank::GetTileInfo(tempMap[ix][iy][iz]);

				const glm::vec3 center = glm::vec3(
					float(ix) - mapOffset,
					float(iz) + 0.5f,
					float(iy) - mapOffset);

				BlockModelInfo blockModelInfo{};
				blockModelInfo.color = id.color;
				blockModelInfo.center = center;
				blockModelInfo.rotate = getRotateAngle(id.rotate);
				setVisibleBlock(id, blockModelInfo, ix, iy, iz);
				blockModelInfo.modelPath = getFileNameBlock(id.type);

				size_t idWall  = addMeshInfo(meshInfo, id.textureWall);
				size_t idFloor = addMeshInfo(meshInfo, id.textureFloor);
				size_t idCeil  = addMeshInfo(meshInfo, id.textureCeil);

				AddObjModel(blockModelInfo, meshInfo[idWall], meshInfo[idCeil], meshInfo[idFloor]);
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
bool testVisBlock(TileGeometryType tile, size_t x, size_t y, size_t z)
{
	if ((x >= MAPCHUNKSIZE) || (y >= MAPCHUNKSIZE) || (z >= MAPCHUNKSIZE)) return false;
	if (tempMap[x][y][z] == NoTile) return false;

	if (tile == TileGeometryType::Block00)
	{
		const auto& b = TileBank::GetTileInfo(tempMap[x][y][z]);
		if (b.type == TileGeometryType::Block00)
		{
			return true;
		}
		else
		{
			// TODO: другие варианты блоков
		}
	}
	return false;
}
//=============================================================================
void MapChunk::setVisibleBlock(const TileInfo& ti, BlockModelInfo& blockModelInfo, size_t x, size_t y, size_t z)
{
	if (x > 0) blockModelInfo.rightVisible = !testVisBlock(ti.type, x - 1, y, z);
	if (x < MAPCHUNKSIZE - 1) blockModelInfo.leftVisible = !testVisBlock(ti.type, x + 1, y, z);

	if (y > 0) blockModelInfo.forwardVisible = !testVisBlock(ti.type, x, y - 1, z);
	if (y < MAPCHUNKSIZE - 1) blockModelInfo.backVisible = !testVisBlock(ti.type, x, y + 1, z);

	if (z > 0) blockModelInfo.bottomVisible = !testVisBlock(ti.type, x, y, z - 1);
	if (z < MAPCHUNKSIZE - 1) blockModelInfo.topVisible = !testVisBlock(ti.type, x, y, z + 1);
}
//=============================================================================