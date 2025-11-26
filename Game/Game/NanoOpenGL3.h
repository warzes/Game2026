#pragma once

//=============================================================================
// Enums
//=============================================================================

enum class PixelFormat : uint8_t
{ 
	Red,
	Rg,
	Rgb,
	Rgba,
	DepthComponent,
	DepthStencil
};

enum class InternalFormat : uint8_t
{
	// Red channel
	R8,   R8_SNORM,  R8UI,  R8I,
	R16,  R16_SNORM, R16UI, R16I,
	R16F, R32F,

	// RG
	RG8,   RG8_SNORM,  RG8UI,  RG8I,
	RG16,  RG16_SNORM, RG16UI, RG16I,
	RG16F, RG32F,

	// RGB
	RGB8,       SRGB8,  RGB565,
	RGB8_SNORM, RGB16F, RGB32F,

	// RGBA
	RGBA8, SRGB8_ALPHA8, RGBA4, RGB5_A1, RGBA8_SNORM, RGBA16F, RGBA32F,

	// Integer formats
	R11F_G11F_B10F,

	// Depth/stencil
	DepthComponent16, DepthComponent24, DepthComponent32F,
	Depth24Stencil8
};

// TODO: не смешивать с DataType и аналогами. Да, оно общее - но в каждом случае только часть будет использоваться. лучше сразу отказаться
enum class PixelType : uint8_t
{
	UnsignedByte,
	UnsignedShort,
	UnsignedInt,
	Float,
	UnsignedInt24_8
};

enum class TextureTarget : uint8_t
{ 
	Texture1D,
	Texture2D,
	Texture3D,
	CubeMap
};

enum class BufferUsage : uint8_t
{
	StaticDraw,
	DynamicDraw,
	StreamDraw,
	StaticRead,
	DynamicRead,
	StreamRead,
	StaticCopy,
	DynamicCopy,
	StreamCopy
};

enum class BufferTarget : uint8_t
{
	Array,
	ElementArray,
	Uniform
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
	OneMinusDstAlpha
};

enum class CullFace : uint8_t
{
	Front = 0,
	Back,
	FrontAndBack
};

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
GLenum GetColorFormatGL(ColorFormat format);

enum class DataType : uint8_t
{
	UnsignedByte,
	Float
};
GLenum GetDataTypeGL(DataType dataType);

//=============================================================================
// Object Handles
//=============================================================================
struct ProgramHandle final { GLuint handle{ 0u }; };
struct BufferHandle final { GLuint handle{ 0u }; };
struct VertexArrayHandle final { GLuint handle{ 0u }; };
struct TextureHandle final { GLuint handle{ 0u }; };
struct SamplerHandle final { GLuint handle{ 0u }; };

struct RenderBufferHandle { GLuint handle{ 0u }; };
struct FrameBufferHandle { GLuint handle{ 0u }; };

//=============================================================================
// Shader Program
//=============================================================================
std::string LoadShaderCode(const std::string& filename, const std::vector<std::string>& defines = {});

ProgramHandle CreateShaderProgram(std::string_view vertexShader);
ProgramHandle CreateShaderProgram(std::string_view vertexShader, std::string_view fragmentShader);
ProgramHandle CreateShaderProgram(std::string_view vertexShader, std::string_view geometryShader, std::string_view fragmentShader);

ProgramHandle LoadShaderProgram(const std::string& vsFile, const std::vector<std::string>& defines = {});
ProgramHandle LoadShaderProgram(const std::string& vsFile, const std::string& fsFile, const std::vector<std::string>& defines = {});
ProgramHandle LoadShaderProgram(const std::string& vsFile, const std::string& gsFile, const std::string& fsFile, const std::vector<std::string>& defines = {});

//=============================================================================
// Shader Uniforms
//=============================================================================
int GetUniformLocation(ProgramHandle program, std::string_view name);

void SetUniform(int id, bool b);
void SetUniform(int id, float s);
void SetUniform(int id, int s);
void SetUniform(int id, unsigned s);
void SetUniform(int id, const glm::vec2& v);
void SetUniform(int id, std::span<const glm::vec2> v);
void SetUniform(int id, const glm::vec3& v);
void SetUniform(int id, std::span<const glm::vec3> v);
void SetUniform(int id, const glm::vec4& v);
void SetUniform(int id, std::span<const glm::vec4> v);
void SetUniform(int id, const glm::quat& v);
void SetUniform(int id, const glm::mat3& m);
void SetUniform(int id, const glm::mat4& m);

//=============================================================================
// VertexAttribute
//=============================================================================

struct VertexAttribute final
{
	GLenum      type;
	GLint       size;
	const void* offset; //  EXAMPLE: (void*)offsetof(Vertex, uv)
	bool        normalized{ false };
	bool        perInstance{ false };
};
void SpecifyVertexAttributes(size_t vertexSize, std::span<const VertexAttribute> attributes);

//=============================================================================
// Vertex Formats
//=============================================================================

struct VertexP3 final
{
	glm::vec3 position;

	static void SetVertexAttributes();
};

struct VertexPNT final
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;

	static void SetVertexAttributes();
};

struct QuadVertex final
{
	glm::vec2 position{ 0.0f };
	glm::vec2 texCoord{ 0.0f };

	static void SetVertexAttributes();
};

struct MeshVertex final
{
	glm::vec3 position{ 0.0f };
	glm::vec3 color{ 1.0f };
	glm::vec3 normal{ 0.0f };
	glm::vec2 texCoord{ 0.0f };
	glm::vec3 tangent{ 0.0f };
	glm::vec3 bitangent{ 0.0f };

	static void SetVertexAttributes();
};

//=============================================================================
// Buffer
//=============================================================================

BufferHandle CreateBuffer(BufferTarget target, BufferUsage usage, size_t size, const void* data);

void BufferSubData(BufferHandle bufferId, BufferTarget target, GLintptr offset, GLsizeiptr size, const void* data);

//=============================================================================
// Textures
//=============================================================================
GLuint LoadTexture2D(std::string_view path, bool gammaCorrection, bool flipVertically = false); // TODO: delete

GLuint CreateTexture2D(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels = nullptr);

void BindTexture2D(GLenum id, GLuint texture);

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

GLuint CreateSamplerState(const SamplerStateInfo& info);

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
GLuint GetCurrentBuffer(GLenum target);
GLuint GetCurrentTexture(GLenum target);

void EnableSRGB(bool enable);

//=============================================================================
// Draw Command
//=============================================================================
void DrawArrays(GLuint vao, GLenum mode, GLint first, GLsizei count);
void DrawElements(GLuint vao, GLenum mode, GLsizei count, GLenum type, const void* indices = nullptr);

//=============================================================================
// OpenGL System
//=============================================================================
namespace oglSystem
{
	bool Init();
	void Close();
} // namespace oglSystem