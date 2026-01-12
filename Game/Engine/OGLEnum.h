#pragma once

//=============================================================================
// Enum
//=============================================================================
enum class PixelFormat : uint8_t
{
	None,
	Red,
	Rg,
	Rgb,
	Bgr,
	Rgba,
	Bgra,
	Stencil,
	DepthComponent,
	DepthStencil
};

enum class InternalFormat : uint8_t
{
	// Red channel
	R8, /*R8_SNORM, R8UI, R8I,
	R16, R16_SNORM, R16UI, R16I,
	R16F, R32F,*/

	// RG
	RG8, /*RG8_SNORM, RG8UI, RG8I,
	RG16, RG16_SNORM, RG16UI, RG16I,
	RG16F, RG32F,*/

	// RGB
	RGB8, SRGB8,
	//RGB8_SNORM, RGB16F, RGB32F,

	// RGBA
	RGBA8, SRGB8_ALPHA8, /*RGBA4, RGB5_A1, RGBA8_SNORM, RGBA16F, RGBA32F,*/

	// Integer formats
	//R11F_G11F_B10F,

	// Depth/stencil
	//DepthComponent16, DepthComponent24, DepthComponent32F,
	//Depth24Stencil8
};

enum class PixelType : uint8_t
{
	UnsignedByte,  Byte,
	UnsignedShort, Short,
	UnsignedInt,   Int,
	Float,
};

enum class PrimitiveMode : uint8_t
{
	Points,
	Lines,
	LineLoop,
	LineStrip,
	Triangles,
	TriangleStrip,
	TriangleFan,
	LinesAdjacency,
	LineStripAdjacency,
	TrianglesAdjacency,
	TriangleStripAdjacency,
};

enum class RasterizationMode : uint8_t
{
	Point,
	Line,
	Fill
};

//=============================================================================
// Function declarations
//=============================================================================
[[nodiscard]] inline GLenum EnumToValue(PixelFormat format) noexcept;
[[nodiscard]] inline GLint EnumToValue(InternalFormat format) noexcept;
[[nodiscard]] inline GLenum EnumToValue(PixelType format) noexcept;

[[nodiscard]] inline GLenum EnumToValue(PrimitiveMode mode) noexcept;
[[nodiscard]] inline GLenum EnumToValue(RasterizationMode mode) noexcept;

//=============================================================================
// Implementation
//=============================================================================
#include "OGLEnum.inl"