#pragma once

//=============================================================================
// Enum
//=============================================================================
enum class DataType : uint8_t
{
	Byte,
	UnsignedByte,
	Short,
	UnsignedShort,
	Int,
	UnsignedInt,
	Float,
	Double
};

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

enum class ComparisonFunc : uint8_t
{
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always
};

enum class Operation : uint8_t
{
	Zero,
	Keep,
	Replace,
	Increment,
	IncrementWrap,
	Decrement,
	DecrementWrap,
	Invert
};

enum class BlendFactor : uint8_t
{
	Zero = 0,
	One,
	SrcColor,
	OneMinusSrcColor,
	DstColor,
	OneMinusDstColor,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstAlpha,
	OneMinusDstAlpha,

	ConstantColor,
	OneMinusConstantColor,
	ConstantAlpha,
	OneMinusConstantAlpha,
	SrcAlphaSaturate,
	Src1Color,
	OneMinusSrc1Color,
	Src1Alpha,
	OneMinusSrc1Alpha
};

enum class BlendEquation : uint8_t
{
	FuncAdd,
	FuncSubtract,
	FuncReverseSubtract,
	Min,
	Max
};

enum class CullFace : uint8_t
{
	Front = 0,
	Back,
	FrontAndBack
};

//=============================================================================
// Function declarations
//=============================================================================
inline GLenum EnumToValue(DataType type) noexcept;
inline GLenum EnumToValue(PixelFormat format) noexcept;
inline GLint EnumToValue(InternalFormat format) noexcept;
inline GLenum EnumToValue(PixelType format) noexcept;
inline GLenum EnumToValue(PrimitiveMode mode) noexcept;
inline GLenum EnumToValue(RasterizationMode mode) noexcept;
inline GLenum EnumToValue(ComparisonFunc func) noexcept;
inline GLenum EnumToValue(Operation op) noexcept;
inline GLenum EnumToValue(BlendFactor factor) noexcept;
inline GLenum EnumToValue(BlendEquation eq) noexcept;

//=============================================================================
// Implementation
//=============================================================================
#include "OGLEnum.inl"