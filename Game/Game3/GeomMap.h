#pragma once

#include "GameModel.h"

struct BlockModelInfo;
struct TileInfo;
class Map;

class MapChunk final
{
public:
	bool Init(Map& map);
	void Close();

	void RecreateBuffer(Map& map);

	GameModel* GetModel() noexcept { return &m_model; }
	size_t GetVertexCount() const { return m_vertCount; }
	size_t GetIndexCount() const { return m_indexCount; }
private:
	void generateBufferMap(Map& map);
	void setVisibleBlock(Map& map, const TileInfo& ti, BlockModelInfo& blockModelInfo, size_t x, size_t y, size_t z);

	GameModel m_model;

	size_t m_vertCount;
	size_t m_indexCount;
};