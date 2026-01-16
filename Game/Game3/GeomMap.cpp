#include "stdafx.h"
#include "GeomMap.h"
#include "GeomTileMap.h"
#include "MapLoadObjTile.h"
#include "Map.h"
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
bool MapChunk::Init(Map& map)
{
	TileInfo tempTile;
	tempTile.type = TileGeometryType::Block00;
	tempTile.textureWall = textures::LoadTexture2D("data/tiles/grass01_wall.png", ColorSpace::Linear, true);
	tempTile.textureCeil = textures::LoadTexture2D("data/tiles/grass01_ceil.png");
	tempTile.textureFloor = textures::LoadTexture2D("data/tiles/grass01.png");

	for (size_t x = 0; x < 11; x++)
	{
		for (size_t y = 0; y < 11; y++)
		{
			map.SetGeomTile(TileBank::AddTileInfo(tempTile), x, 14 + y, 0);
		}
	}

	tempTile.type = TileGeometryType::Block01;
	tempTile.textureWall = textures::LoadTexture2D("data/tiles/grass01_wall.png", ColorSpace::Linear, true);
	tempTile.textureCeil = textures::LoadTexture2D("data/tiles/grass01_ceil.png");
	tempTile.textureFloor = textures::LoadTexture2D("data/tiles/grass01.png");
	tempTile.rotate = RotateAngleY::Rotate270;
	map.SetGeomTile(TileBank::AddTileInfo(tempTile), 16, 17, 1);
	tempTile.rotate = RotateAngleY::Rotate0;
	map.SetGeomTile(TileBank::AddTileInfo(tempTile), 17, 16, 1);
	tempTile.rotate = RotateAngleY::Rotate180;
	map.SetGeomTile(TileBank::AddTileInfo(tempTile), 17, 18, 1);
	tempTile.rotate = RotateAngleY::Rotate90;
	map.SetGeomTile(TileBank::AddTileInfo(tempTile), 18, 17, 1);
	tempTile.type = TileGeometryType::Block00;
	tempTile.rotate = RotateAngleY::Rotate0;
	map.SetGeomTile(TileBank::AddTileInfo(tempTile), 17, 17, 1);

	generateBufferMap(map);

	return true;
}
//=============================================================================
void MapChunk::Close()
{
}
//=============================================================================
void MapChunk::RecreateBuffer(Map& map)
{
	m_model.model.Free();
	generateBufferMap(map);
}
//=============================================================================
void MapChunk::generateBufferMap(Map& map)
{
	std::vector<MeshInfo> meshInfo;
	m_vertCount = 0;
	m_indexCount = 0;

	const float mapOffset = MAPCHUNKSIZE / 2.0f;
	for (size_t iy = 0; iy < MAPCHUNKSIZE; iy++)
	{
		for (size_t ix = 0; ix < MAPCHUNKSIZE; ix++)
		{
			for (size_t iz = 0; iz < MAPCHUNKSIZE; iz++)
			{
				if (map.GetGeomTile(ix, iy, iz) == NoTile) continue;

				const auto& id = TileBank::GetTileInfo(map.GetGeomTile(ix, iy, iz));

				const glm::vec3 center = glm::vec3(
					float(ix) - mapOffset,
					float(iz) + 0.5f,
					float(iy) - mapOffset);

				BlockModelInfo blockModelInfo{};
				blockModelInfo.color = id.color;
				blockModelInfo.center = center;
				blockModelInfo.rotate = getRotateAngle(id.rotate);
				setVisibleBlock(map, id, blockModelInfo, ix, iy, iz);
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
		m_vertCount += meshInfo[i].vertices.size();
		m_indexCount += meshInfo[i].indices.size();
	}

	m_model.model.Create(meshInfo);
}
//=============================================================================
bool testVisBlock(Map& map, TileGeometryType tile, size_t x, size_t y, size_t z)
{
	if ((x >= MAPCHUNKSIZE) || (y >= MAPCHUNKSIZE) || (z >= MAPCHUNKSIZE)) return false;
	if (map.GetGeomTile(x, y, z) == NoTile) return false;

	if (tile == TileGeometryType::Block00)
	{
		const auto& b = TileBank::GetTileInfo(map.GetGeomTile(x, y, z));
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
void MapChunk::setVisibleBlock(Map& map, const TileInfo& ti, BlockModelInfo& blockModelInfo, size_t x, size_t y, size_t z)
{
	if (x > 0) blockModelInfo.rightVisible = !testVisBlock(map, ti.type, x - 1, y, z);
	if (x < MAPCHUNKSIZE - 1) blockModelInfo.leftVisible = !testVisBlock(map, ti.type, x + 1, y, z);

	if (y > 0) blockModelInfo.forwardVisible = !testVisBlock(map, ti.type, x, y - 1, z);
	if (y < MAPCHUNKSIZE - 1) blockModelInfo.backVisible = !testVisBlock(map, ti.type, x, y + 1, z);

	if (z > 0) blockModelInfo.bottomVisible = !testVisBlock(map, ti.type, x, y, z - 1);
	if (z < MAPCHUNKSIZE - 1) blockModelInfo.topVisible = !testVisBlock(map, ti.type, x, y, z + 1);
}
//=============================================================================