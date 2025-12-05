#pragma once

#include "NanoOpenGL3Advance.h"

struct Texture2D final
{
	bool operator==(const Texture2D& rhs) const
	{
		return id.handle   == rhs.id.handle
			&& pixelFormat == rhs.pixelFormat
			&& width       == rhs.width
			&& height      == rhs.height;
	}
	bool operator!=(const Texture2D& rhs) const { return !(*this == rhs); }

	Texture2DHandle id{ 0 };
	PixelFormat   pixelFormat{ PixelFormat::None };
	uint32_t      width{ 0 };
	uint32_t      height{ 0 };
};

bool IsValid(Texture2D tex);
void Destroy(Texture2D& tex);

namespace textures
{
	bool Init();
	void Close();

	Texture2D GetWhiteTexture2D();
	Texture2D GetDefaultDiffuse2D();
	Texture2D GetDefaultNormal2D();
	Texture2D GetDefaultSpecular2D();
	Texture2D LoadTexture2D(const std::string& fileName, ColorSpace colorSpace = ColorSpace::Linear, bool flipVertical = false);
	Texture2D CreateTextureFromData(std::string_view name, aiTexture* embTex, ColorSpace colorSpace = ColorSpace::Linear, bool flipVertical = false);
} // namespace textures