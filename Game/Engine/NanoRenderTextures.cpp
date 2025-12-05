#include "stdafx.h"
#include "NanoRenderTextures.h"
#include "NanoCore.h"
#include "NanoLog.h"
#include "NanoIO.h"
//=============================================================================
struct TextureCache final
{
	bool operator==(const TextureCache&) const noexcept = default;

	std::string name;
	bool sRGB;
	bool flipVertical;
};
//=============================================================================
namespace std
{
	template<>
	struct hash<TextureCache>
	{
		std::size_t operator()(const TextureCache& tc) const noexcept
		{
			std::size_t h1 = std::hash<std::string>{}(tc.name);
			std::size_t h2 = std::hash<bool>{}(tc.sRGB);
			std::size_t h3 = std::hash<bool>{}(tc.flipVertical);
			std::size_t seed = 0;
			HashCombine(seed, h1, h2, h3);
			return seed;
		}
	};
}
//=============================================================================
namespace
{
	std::unordered_map<TextureCache, Texture2D> texturesMap;
	Texture2D defaultWhite2D;
	Texture2D defaultDiffuse2D;
	Texture2D defaultNormal2D;
	Texture2D defaultSpecular2D;
}
//=============================================================================
bool IsValid(Texture2D tex)
{
	return IsValid(tex.id) && tex.width > 0 && tex.height > 0 && tex.pixelFormat != PixelFormat::None;
}
//=============================================================================
void Destroy(Texture2D& tex)
{
	Destroy(tex.id);
	tex.pixelFormat = PixelFormat::None;
	tex.width = tex.height = 0;
}
//=============================================================================
bool textures::Init()
{
	GLuint currentTexture = GetCurrentTexture(GL_TEXTURE_2D);

	// Create white texture
	{
		constexpr size_t SizeTexture = 1u;
		uint8_t data[SizeTexture][SizeTexture][3];
		for (size_t i = 0; i < SizeTexture; i++)
		{
			for (size_t j = 0; j < SizeTexture; j++)
			{
				data[i][j][0] = 255;
				data[i][j][1] = 255;
				data[i][j][2] = 255;
			}
		}

		defaultWhite2D.pixelFormat = PixelFormat::Rgb;
		defaultWhite2D.width = SizeTexture;
		defaultWhite2D.height = SizeTexture;
		glGenTextures(1, &defaultWhite2D.id.handle);
		glBindTexture(GL_TEXTURE_2D, defaultWhite2D.id.handle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, SizeTexture, SizeTexture, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	// Create default diffuse texture
	{
		constexpr size_t SizeTexture = 32u;
		uint8_t data[SizeTexture][SizeTexture][3];
		for (size_t i = 0; i < SizeTexture; i++)
		{
			for (size_t j = 0; j < SizeTexture; j++)
			{
				if ((i + j) % 2 == 0)
				{
					data[i][j][0] = 250;
					data[i][j][1] = 70;
					data[i][j][2] = 100;
				}
				else
				{
					data[i][j][0] = 150;
					data[i][j][1] = 50;
					data[i][j][2] = 200;
				}
			}
		}

		defaultDiffuse2D.pixelFormat = PixelFormat::Rgb;
		defaultDiffuse2D.width = SizeTexture;
		defaultDiffuse2D.height = SizeTexture;
		glGenTextures(1, &defaultDiffuse2D.id.handle);
		glBindTexture(GL_TEXTURE_2D, defaultDiffuse2D.id.handle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, SizeTexture, SizeTexture, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	// Create default normal texture
	{
		constexpr size_t SizeTexture = 8u;
		uint8_t data[SizeTexture][SizeTexture][3];
		for (size_t i = 0; i < SizeTexture; i++)
		{
			for (size_t j = 0; j < SizeTexture; j++)
			{
				data[i][j][0] = 128;
				data[i][j][1] = 128;
				data[i][j][2] = 255;
			}
		}

		defaultNormal2D.pixelFormat = PixelFormat::Rgb;
		defaultNormal2D.width = SizeTexture;
		defaultNormal2D.height = SizeTexture;
		glGenTextures(1, &defaultNormal2D.id.handle);
		glBindTexture(GL_TEXTURE_2D, defaultNormal2D.id.handle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, SizeTexture, SizeTexture, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	// Create default specular texture
	{
		constexpr size_t SizeTexture = 8u;
		uint8_t data[SizeTexture][SizeTexture][3];
		for (size_t i = 0; i < SizeTexture; i++)
		{
			for (size_t j = 0; j < SizeTexture; j++)
			{
				data[i][j][0] = 255;//Roughness
				data[i][j][1] = 255;//Metallic
				data[i][j][2] = 0;
			}
		}

		defaultSpecular2D.pixelFormat = PixelFormat::Rgb;
		defaultSpecular2D.width = SizeTexture;
		defaultSpecular2D.height = SizeTexture;
		glGenTextures(1, &defaultSpecular2D.id.handle);
		glBindTexture(GL_TEXTURE_2D, defaultSpecular2D.id.handle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, SizeTexture, SizeTexture, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return true;
}
//=============================================================================
void textures::Close()
{
	Destroy(defaultWhite2D.id);
	Destroy(defaultDiffuse2D.id);
	Destroy(defaultNormal2D.id);
	Destroy(defaultSpecular2D.id);

	for (auto& it : texturesMap)
	{
		Destroy(it.second.id);
	}
	texturesMap.clear();
}
//=============================================================================
Texture2D textures::GetWhiteTexture2D()
{
	return defaultWhite2D;
}
//=============================================================================
Texture2D textures::GetDefaultDiffuse2D()
{
	return defaultDiffuse2D;
}
//=============================================================================
Texture2D textures::GetDefaultNormal2D()
{
	return defaultNormal2D;
}
//=============================================================================
Texture2D textures::GetDefaultSpecular2D()
{
	return defaultSpecular2D;
}
//=============================================================================
Texture2D textures::LoadTexture2D(const std::string& fileName, ColorSpace colorSpace, bool flipVertical)
{
	TextureCache keyMap = { .name = fileName, .sRGB = colorSpace == ColorSpace::sRGB, .flipVertical = flipVertical};
	auto it = texturesMap.find(keyMap);
	if (it != texturesMap.end() && IsValid(it->second))
	{
		return it->second;
	}
	else
	{
		bool hasTex = io::Exists(fileName);
		if (hasTex == false)
		{
			Error("Failed to load texture " + fileName);
			return GetDefaultDiffuse2D();
		}

		stbi_set_flip_vertically_on_load(flipVertical);

		int width, height, nrComponents;
		stbi_uc* pixels = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);
		if (!pixels || nrComponents < 1 || nrComponents > 4 || width <= 0 || height <= 0)
		{
			stbi_image_free(pixels);
			Error("Failed to load texture " + fileName);
			return GetDefaultDiffuse2D();
		}

		InternalFormat internalFormat{};
		PixelFormat pixelFormat{ PixelFormat::None };
		if (nrComponents == 1)
		{
			internalFormat = InternalFormat::R8;
			pixelFormat = PixelFormat::Red;
		}
		else if (nrComponents == 2)
		{
			internalFormat = InternalFormat::RG8;
			pixelFormat = PixelFormat::Rg;
		}
		else if (nrComponents == 3)
		{
			internalFormat = (colorSpace == ColorSpace::sRGB) ? InternalFormat::SRGB8 : InternalFormat::RGB8;
			pixelFormat = PixelFormat::Rgb;
		}
		else if (nrComponents == 4)
		{
			internalFormat = (colorSpace == ColorSpace::sRGB) ? InternalFormat::SRGB8_ALPHA8 : InternalFormat::RGBA8;
			pixelFormat = PixelFormat::Rgba;
		}
		else
		{
			std::unreachable();
		}

		Texture2DHandle textureID = CreateTexture2D(static_cast<uint32_t>(width), static_cast<uint32_t>(height), internalFormat, pixelFormat, PixelType::UnsignedByte, pixels);
		TextureConfig texConfig{
			.minFilter = TextureFilter::LinearMipmapLinear,
			.magFilter = TextureFilter::Linear,
			.wrapS = TextureWrap::Repeat,
			.wrapT = TextureWrap::Repeat,
			.generateMipmaps = true
		};
		SetTextureParameters(textureID, texConfig);

		stbi_image_free(pixels);

		Debug("Load Texture: " + fileName);

		texturesMap[keyMap] = Texture2D{ 
			.id = textureID, 
			.pixelFormat = pixelFormat,
			.width = static_cast<uint32_t>(width), 
			.height = static_cast<uint32_t>(height)
		};
		return texturesMap[keyMap];
	}
}
//=============================================================================
Texture2D textures::CreateTextureFromData(std::string_view name, aiTexture* embTex, ColorSpace colorSpace, bool flipVertical)
{
	TextureCache keyMap = { .name = name.data(), .sRGB = colorSpace == ColorSpace::sRGB, .flipVertical = flipVertical };
	auto it = texturesMap.find(keyMap);
	if (it != texturesMap.end())
	{
		return it->second;
	}
	else
	{
		stbi_set_flip_vertically_on_load(flipVertical);

		int width, height, nrComponents;
		stbi_uc* data{ nullptr };
		if (embTex->mHeight == 0)
			data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(embTex->pcData), static_cast<int>(embTex->mWidth), &width, &height, &nrComponents, 0);
		else
			data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(embTex->pcData), static_cast<int>(embTex->mWidth * embTex->mHeight), &width, &height, &nrComponents, 0);
		if (!data || nrComponents < 1 || nrComponents > 4 || width <= 0 || height <= 0)
		{
			stbi_image_free(data);
			Error("Error while trying to load embedded texture!");
			return GetDefaultDiffuse2D();
		}

		InternalFormat internalFormat{};
		PixelFormat pixelFormat{ PixelFormat::None };
		if (nrComponents == 1)
		{
			internalFormat = InternalFormat::R8;
			pixelFormat = PixelFormat::Red;
		}
		else if (nrComponents == 2)
		{
			internalFormat = InternalFormat::RG8;
			pixelFormat = PixelFormat::Rg;
		}
		else if (nrComponents == 3)
		{
			internalFormat = (colorSpace == ColorSpace::sRGB) ? InternalFormat::SRGB8 : InternalFormat::RGB8;
			pixelFormat = PixelFormat::Rgb;
		}
		else if (nrComponents == 4)
		{
			internalFormat = (colorSpace == ColorSpace::sRGB) ? InternalFormat::SRGB8_ALPHA8 : InternalFormat::RGBA8;
			pixelFormat = PixelFormat::Rgba;
		}
		else
		{
			std::unreachable();
		}

		Texture2DHandle textureID = CreateTexture2D(static_cast<uint32_t>(width), static_cast<uint32_t>(height), internalFormat, pixelFormat, PixelType::UnsignedByte, data);
		TextureConfig texConfig{
			.minFilter = TextureFilter::LinearMipmapLinear,
			.magFilter = TextureFilter::Linear,
			.wrapS = TextureWrap::Repeat,
			.wrapT = TextureWrap::Repeat,
			.generateMipmaps = true
		};
		SetTextureParameters(textureID, texConfig);
		
		stbi_image_free(data);

		Debug("Load Texture: " + std::string(name));
		texturesMap[keyMap] = Texture2D{ .id = textureID, .width = (uint32_t)width, .height = (uint32_t)height };
		return texturesMap[keyMap];
	}
}
//=============================================================================