#pragma once

#include "GeomTileMap.h"

constexpr const size_t MAPCHUNKSIZE = 30;

/*
TEMP: я сейчас храню информацию о тайле только геометрическую. а возможно нужна будет еще логическая-игровая.
*/
struct LogicTile final
{
	int t;
};

struct TileSelection final
{
	size_t x{ 0 };
	size_t y{ 0 };
	size_t z{ 0 };

	size_t tile{ NoTile };
};

class Map final
{
public:
	Map();

	void Clear();
	void ClearGeomTile(size_t x, size_t y, size_t z);
	void SetGeomTile(size_t tile, size_t x, size_t y, size_t z);
	size_t GetGeomTile(size_t x, size_t y, size_t z) const;

	// находится ли позиция внутри карты
	bool IsInBounds(size_t x, size_t y, size_t z) const;

	// Найти тайл, над которым находится курсор 
	TileSelection RaycastTile(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const;

	bool SaveToFile(const std::string& filename) const;
	bool LoadFromFile(const std::string& filename);

private:
	size_t m_geomMap[MAPCHUNKSIZE][MAPCHUNKSIZE][MAPCHUNKSIZE] = { 0 };
	LogicTile m_logicMap[MAPCHUNKSIZE][MAPCHUNKSIZE][MAPCHUNKSIZE] = {};
};