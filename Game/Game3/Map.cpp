#include "stdafx.h"
#include "Map.h"
//=============================================================================
Map::Map()
{
	Clear();
}
//=============================================================================
void Map::Clear()
{
	for (size_t y = 0; y < MAPCHUNKSIZE; y++)
	{
		for (size_t x = 0; x < MAPCHUNKSIZE; x++)
		{
			for (size_t z = 0; z < MAPCHUNKSIZE; z++)
			{
				m_geomMap[x][y][z] = NoTile;
			}
		}
	}
}
//=============================================================================
void Map::ClearGeomTile(size_t x, size_t y, size_t z)
{
	SetGeomTile(NoTile, x, y, z);
}
//=============================================================================
void Map::SetGeomTile(size_t tile, size_t x, size_t y, size_t z)
{
	if (IsInBounds(x, y, z)) 
		m_geomMap[x][y][z] = tile;
}
//=============================================================================
size_t Map::GetGeomTile(size_t x, size_t y, size_t z) const
{
	if (IsInBounds(x, y, z)) 
		return m_geomMap[x][y][z];
	
	return NoTile;
}
//=============================================================================
bool Map::IsInBounds(size_t x, size_t y, size_t z) const
{
	if (x < MAPCHUNKSIZE && y < MAPCHUNKSIZE && z < MAPCHUNKSIZE) return true;
	else return false;
}
//=============================================================================
TileSelection Map::RaycastTile(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const
{
	// Алгоритм лучевого пересечения с воксельной сеткой (алгоритм цифровой дифференциальной аналого-вычислительной машины)
	glm::ivec3 currentPos = glm::ivec3(
		static_cast<int>(std::floor(rayOrigin.x)),
		static_cast<int>(std::floor(rayOrigin.y)),
		static_cast<int>(std::floor(rayOrigin.z))
	);

	// Определяем шаги
	glm::ivec3 step(
		rayDirection.x > 0 ? 1 : (rayDirection.x < 0 ? -1 : 0),
		rayDirection.y > 0 ? 1 : (rayDirection.y < 0 ? -1 : 0),
		rayDirection.z > 0 ? 1 : (rayDirection.z < 0 ? -1 : 0)
	);

	// Вычисляем расстояния до следующих плоскостей
	glm::vec3 tMax(
		(step.x > 0) ? (currentPos.x + 1 - rayOrigin.x) / rayDirection.x :
		(rayDirection.x != 0 ? (currentPos.x - rayOrigin.x) / rayDirection.x : std::numeric_limits<float>::max()),
		(step.y > 0) ? (currentPos.y + 1 - rayOrigin.y) / rayDirection.y :
		(rayDirection.y != 0 ? (currentPos.y - rayOrigin.y) / rayDirection.y : std::numeric_limits<float>::max()),
		(step.z > 0) ? (currentPos.z + 1 - rayOrigin.z) / rayDirection.z :
		(rayDirection.z != 0 ? (currentPos.z - rayOrigin.z) / rayDirection.z : std::numeric_limits<float>::max())
	);

	// Вычисляем шаги по осям
	glm::vec3 tDelta(
		(rayDirection.x != 0) ? std::abs(1.0f / rayDirection.x) : std::numeric_limits<float>::max(),
		(rayDirection.y != 0) ? std::abs(1.0f / rayDirection.y) : std::numeric_limits<float>::max(),
		(rayDirection.z != 0) ? std::abs(1.0f / rayDirection.z) : std::numeric_limits<float>::max()
	);

	const float mapOffset = MAPCHUNKSIZE / 2.0f;
	currentPos.x += mapOffset;
	currentPos.y -= 0.5;
	currentPos.z += mapOffset;

	// Проходим по вокселям, пока не найдем блок или не выйдем за границы
	for (int i = 0; i < MAPCHUNKSIZE * MAPCHUNKSIZE * MAPCHUNKSIZE; ++i)
	{
		// Проверяем, находится ли текущая позиция в пределах карты
		if (IsInBounds(currentPos.x, currentPos.y, currentPos.z))
		{
			size_t tile = GetGeomTile(currentPos.x, currentPos.y, currentPos.z);
			// Если в этой позиции есть блок, возвращаем его
			if (tile != NoTile)
			{
				TileSelection sel;
				sel.tile = tile;
				sel.x = currentPos.x;
				sel.y = currentPos.y;
				sel.z = currentPos.z;
				return sel;
			}
		}
		else
		{
			// Если вышли за границы, прекращаем поиск
			break;
		}

		// Определяем, по какой оси двигаться дальше
		if (tMax.x < tMax.y && tMax.x < tMax.z)
		{
			if (tMax.x > 100.0f) break; // Ограничение дистанции
			currentPos.x += step.x;
			tMax.x += tDelta.x;
		}
		else if (tMax.y < tMax.z) {
			if (tMax.y > 100.0f) break; // Ограничение дистанции
			currentPos.y += step.y;
			tMax.y += tDelta.y;
		}
		else {
			if (tMax.z > 100.0f) break; // Ограничение дистанции
			currentPos.z += step.z;
			tMax.z += tDelta.z;
		}
	}

	// Если блок не найден, возвращаем невалидный результат
	return {};
}
//=============================================================================
bool Map::SaveToFile(const std::string& filename) const
{
	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		Error("Could not open file for writing: " + filename);
		return false;
	}

	// Записываем размеры карты
	file.write(reinterpret_cast<const char*>(&MAPCHUNKSIZE), sizeof(int));
	file.write(reinterpret_cast<const char*>(&MAPCHUNKSIZE), sizeof(int));
	file.write(reinterpret_cast<const char*>(&MAPCHUNKSIZE), sizeof(int));

	// Записываем данные блоков
	for (int x = 0; x < MAPCHUNKSIZE; ++x)
	{
		for (int y = 0; y < MAPCHUNKSIZE; ++y)
		{
			for (int z = 0; z < MAPCHUNKSIZE; ++z)
			{
				auto tile = GetGeomTile(x, y, z);
				file.write(reinterpret_cast<const char*>(&tile), sizeof(size_t));
			}
		}
	}

	file.close();
	return true;
}
//=============================================================================
bool Map::LoadFromFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		Error("Could not open file for reading: " + filename);
		return false;
	}

	// Читаем размеры карты
	int sizeX, sizeY, sizeZ;
	file.read(reinterpret_cast<char*>(&sizeX), sizeof(int));
	file.read(reinterpret_cast<char*>(&sizeY), sizeof(int));
	file.read(reinterpret_cast<char*>(&sizeZ), sizeof(int));

	// Проверяем совместимость размеров
	if (sizeX != MAPCHUNKSIZE || sizeY != MAPCHUNKSIZE || sizeZ != MAPCHUNKSIZE)
	{
		Error("Map dimensions in file don't match expected dimensions.");
		file.close();
		return false;
	}

	// Читаем данные блоков
	for (int x = 0; x < MAPCHUNKSIZE; ++x)
	{
		for (int y = 0; y < MAPCHUNKSIZE; ++y)
		{
			for (int z = 0; z < MAPCHUNKSIZE; ++z)
			{
				size_t tile;
				file.read(reinterpret_cast<char*>(&tile), sizeof(size_t));
				SetGeomTile(tile, x, y, z);
			}
		}
	}

	file.close();
	return true;
}
//=============================================================================