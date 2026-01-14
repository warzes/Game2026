#include "stdafx.h"
#include "TileMap.h"
//=============================================================================
std::vector<TileInfo> TileInfoCache;
//=============================================================================
size_t TileBank::AddTileInfo(const TileInfo& temp)
{
	for (size_t i = 0; i < TileInfoCache.size(); i++)
	{
		if (TileInfoCache[i] == temp)
		{
			return i;
		}
	}
	TileInfoCache.push_back(temp);
	return TileInfoCache.size() - 1;
}
//=============================================================================
const TileInfo& TileBank::GetTileInfo(size_t id)
{
	assert(id < TileInfoCache.size());
	return TileInfoCache[id];
}
//=============================================================================