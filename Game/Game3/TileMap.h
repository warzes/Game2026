#pragma once

enum class RotateAngleY : uint8_t
{
	Rotate0,
	Rotate90,
	Rotate180,
	Rotate270,
};

enum class TileGeometryType
{
	None,

	Block00,
	Block01,
};

struct TileInfo final
{
	bool operator==(const TileInfo&) const noexcept = default;

	TileGeometryType type{ TileGeometryType::None };
	glm::vec4 color{ 1.0f };
	Texture2D textureFloor;
	Texture2D textureCeil;
	Texture2D textureWall;
	RotateAngleY rotate{ RotateAngleY::Rotate0 };
};

constexpr size_t NoTile = std::numeric_limits<size_t>::max();

namespace TileBank
{
	size_t AddTileInfo(const TileInfo& temp);

	TileInfo* GetTileInfo(size_t id);
} // namespace TileBank