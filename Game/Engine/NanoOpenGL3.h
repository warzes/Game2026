#pragma once

#include "OGLEnum.h"

//=============================================================================
// Enums
//=============================================================================

enum class PolygonMode : uint8_t
{
	Point = 0,
	Line,
	Fill
};

enum class TextureFilter : uint8_t
{
	Nearest,
	Linear,
	NearestMipmapNearest,
	LinearMipmapNearest,
	NearestMipmapLinear,
	LinearMipmapLinear
};

enum class TextureWrap : uint8_t
{
	Repeat,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder
};

enum class ColorSpace : uint8_t
{
	Linear,
	sRGB
};

enum class ColorFormat : uint8_t
{
	Red,
	RG,
	RGB,
	RGBA
};
GLenum GetColorFormatGL(ColorFormat format) noexcept;

//=============================================================================
// Object Handles
//=============================================================================
struct VertexArrayHandle final { GLuint handle{ 0u }; };
struct Texture1DHandle final { GLuint handle{ 0u }; };
struct Texture2DHandle final { GLuint handle{ 0u }; };
struct Texture3DHandle final { GLuint handle{ 0u }; };
struct TextureCubeHandle final { GLuint handle{ 0u }; };
struct Texture1DArrayHandle final { GLuint handle{ 0u }; };
struct Texture2DArrayHandle final { GLuint handle{ 0u }; };
struct SamplerHandle final { GLuint handle{ 0u }; };

struct RenderBufferHandle { GLuint handle{ 0u }; };
struct FrameBufferHandle { GLuint handle{ 0u }; };

//=============================================================================
// Textures
//=============================================================================

// Функции для создания текстур
Texture1DHandle CreateTexture1D(unsigned width, InternalFormat internalformat, PixelFormat format, PixelType type, const void* pixels = nullptr);
Texture2DHandle CreateTexture2D(unsigned width, unsigned height, InternalFormat internalformat, PixelFormat format, PixelType type, const void* pixels = nullptr);
Texture3DHandle CreateTexture3D(unsigned width, unsigned height, unsigned depth, InternalFormat internalformat, PixelFormat format, PixelType type, const void* pixels = nullptr);
Texture1DArrayHandle CreateTexture1DArray(InternalFormat internalformat, unsigned width, unsigned arraySize, PixelFormat format, PixelType type, const void* pixels = nullptr);
Texture2DArrayHandle CreateTexture2DArray(InternalFormat internalformat, unsigned width, unsigned height, unsigned arraySize, PixelFormat format, PixelType type, const void* pixels = nullptr);
TextureCubeHandle CreateCubeTexture(InternalFormat internalformat, unsigned width, unsigned height, PixelFormat format, PixelType type, const void* posX, const void* negX, const void* posY, const void* negY, const void* posZ, const void* negZ);

// Функции для подгрузки данных в текстуры
void SetTextureData(Texture1DHandle texture, InternalFormat internalformat, unsigned width, PixelFormat format, PixelType type, const void* pixels);
void SetTextureData(Texture2DHandle texture, InternalFormat internalformat, unsigned width, unsigned height, PixelFormat format, PixelType type, const void* pixels);
void SetTextureData(Texture3DHandle texture, InternalFormat internalformat, unsigned width, unsigned height, unsigned depth, PixelFormat format, PixelType type, const void* pixels);
void SetTextureData(Texture1DArrayHandle texture, InternalFormat internalformat, unsigned width, unsigned arraySize, PixelFormat format, PixelType type, const void* pixels);
void SetTextureData(Texture2DArrayHandle texture, InternalFormat internalformat, unsigned width, unsigned height, unsigned arraySize, PixelFormat format, PixelType type, const void* pixels);
void SetTextureData(TextureCubeHandle texture, InternalFormat internalformat, unsigned width, unsigned height, PixelFormat format, PixelType type, const void* posX, const void* negX, const void* posY, const void* negY, const void* posZ, const void* negZ);

void BindTexture2D(GLenum id, Texture2DHandle texture);

bool IsValid(Texture1DHandle id);
bool IsValid(Texture2DHandle id);
bool IsValid(Texture3DHandle id);
bool IsValid(TextureCubeHandle id);
bool IsValid(Texture1DArrayHandle id);
bool IsValid(Texture2DArrayHandle id);

void Destroy(Texture1DHandle& id);
void Destroy(Texture2DHandle& id);
void Destroy(Texture3DHandle& id);
void Destroy(TextureCubeHandle& id);
void Destroy(Texture1DArrayHandle& id);
void Destroy(Texture2DArrayHandle& id);

struct TextureConfig final
{
	TextureFilter minFilter{ TextureFilter::LinearMipmapLinear };
	TextureFilter magFilter{ TextureFilter::Linear };
	TextureWrap   wrapS{ TextureWrap::Repeat };
	TextureWrap   wrapT{ TextureWrap::Repeat };
	TextureWrap   wrapR{ TextureWrap::Repeat };
	bool          generateMipmaps{ true };
};
void SetTextureParameters(Texture1DHandle texture, const TextureConfig& config);
void SetTextureParameters(Texture2DHandle texture, const TextureConfig& config);
void SetTextureParameters(Texture3DHandle texture, const TextureConfig& config);
void SetTextureParameters(TextureCubeHandle texture, const TextureConfig& config);

//=============================================================================
// Sampler
//=============================================================================
struct SamplerStateInfo final
{
	bool operator==(const SamplerStateInfo&) const noexcept = default;

	float          maxAnisotropy{ 1.0f };
	float          minLod{ -1000.0f };
	float          maxLod{ 1000.0f };
	float          mipLodBias{ 0.0f };
	bool           compareEnabled{ false };
	ComparisonFunc comparisonFunc{ ComparisonFunc::Less };
	float          borderColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	TextureFilter  minFilter{ TextureFilter::Linear };
	TextureFilter  magFilter{ TextureFilter::Linear };

	TextureWrap    wrapS{ TextureWrap::Repeat };
	TextureWrap    wrapT{ TextureWrap::Repeat };
	TextureWrap    wrapR{ TextureWrap::Repeat };
};

template<>
struct std::hash<SamplerStateInfo>
{
	std::size_t operator()(const SamplerStateInfo& k) const noexcept;
};

SamplerHandle CreateSamplerState(const SamplerStateInfo& info);

//=============================================================================
// Framebuffer
//=============================================================================


//=============================================================================
// States
//=============================================================================

struct GLState final
{
	struct
	{
		bool enable{ true };
		ComparisonFunc depthFunc{ ComparisonFunc::Less };
		bool depthMask{ true };
	} depthState;

	struct
	{
		bool enable{ false };
		ComparisonFunc frontFunc{ ComparisonFunc::Always };
		ComparisonFunc backFunc{ ComparisonFunc::Always };
		int frontRef{ 0 };
		int backRef{ 0 };
		unsigned int frontMask{ 0xFF };
		unsigned int backMask{ 0xFF };
		// Operations можно добавить при необходимости
	} stencilState;

	struct
	{
		bool enable{ false };
		BlendFactor srcRGB{ BlendFactor::SrcAlpha };
		BlendFactor dstRGB{ BlendFactor::OneMinusSrcAlpha };
		BlendFactor srcAlpha{ BlendFactor::One };
		BlendFactor dstAlpha{ BlendFactor::OneMinusSrcAlpha };
	} blendState;

	struct
	{
		bool enable{ true };
	} multisampleState;

	struct
	{
		bool r{ true };
		bool g{ true };
		bool b{ true };
		bool a{ true };
	} colorMaskState;

	struct
	{
		bool enable{ false };
		CullFace cullFace{ CullFace::Back };
	} cullState;

	struct
	{
		PolygonMode mode{ PolygonMode::Fill };
	} polygonState;
};

// TODO: функция получения стейта из реального OpenGL

void ResetStateDepth();
void ResetStateStencil();
void ResetStateBlend();
void ResetStateMultisample();
void ResetStateColorMaskState();
void ResetStateCullState();
void ResetStatePolygonState();
void ResetStateAll();

void BindState(const GLState& state);

//=============================================================================
// Get GL States
//=============================================================================

GLuint GetCurrentTexture(GLenum target);

void EnableSRGB(bool enable);

//=============================================================================
// Draw Command
//=============================================================================
void DrawArrays(GLuint vao, GLenum mode, GLint first, GLsizei count);
void DrawElements(GLuint vao, GLenum mode, GLsizei count, GLenum type, const void* indices = nullptr);