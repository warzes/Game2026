#pragma once

//=============================================================================
// Enums
//=============================================================================

enum class CompareFunc : uint8_t
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

//=============================================================================
// Shader Program
//=============================================================================
std::string LoadShaderCode(const std::string& filename, const std::vector<std::string>& defines = {});

GLuint CreateShaderProgram(std::string_view vertexShader);
GLuint CreateShaderProgram(std::string_view vertexShader, std::string_view fragmentShader);
GLuint CreateShaderProgram(std::string_view vertexShader, std::string_view geometryShader, std::string_view fragmentShader);

GLuint LoadShaderProgram(const std::string& vsFile);
GLuint LoadShaderProgram(const std::string& vsFile, const std::string& fsFile);
GLuint LoadShaderProgram(const std::string& vsFile, const std::string& gsFile, const std::string& fsFile);

//=============================================================================
// Shader Uniforms
//=============================================================================
int GetUniformLocation(GLuint program, std::string_view name);

void SetUniform(GLuint id, bool b);
void SetUniform(GLuint id, float s);
void SetUniform(GLuint id, int s);
void SetUniform(GLuint id, unsigned s);
void SetUniform(GLuint id, const glm::vec2& v);
void SetUniform(GLuint id, std::span<const glm::vec2> v);
void SetUniform(GLuint id, const glm::vec3& v);
void SetUniform(GLuint id, std::span<const glm::vec3> v);
void SetUniform(GLuint id, const glm::vec4& v);
void SetUniform(GLuint id, std::span<const glm::vec4> v);
void SetUniform(GLuint id, const glm::quat& v);
void SetUniform(GLuint id, const glm::mat3& m);
void SetUniform(GLuint id, const glm::mat4& m);

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

struct VertexP final
{
	glm::vec3 position;
};
void SetVertexPAttributes();

struct VertexPNT final
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};
void SetVertexPNTAttributes();

//=============================================================================
// Buffer
//=============================================================================
enum class BufferUsage : uint8_t
{
	Static,
	Dynamic
};
GLuint CreateBuffer(GLenum target, BufferUsage usage, GLsizeiptr size, const void* data);

void BufferSubData(GLuint bufferId, GLenum target, GLintptr offset, GLsizeiptr size, const void* data);

//=============================================================================
// Textures
//=============================================================================
GLuint LoadTexture2D(std::string_view path, bool gammaCorrection, bool flipVertically = false); // TODO: delete

GLuint CreateTexture2D(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels = nullptr);

void BindTexture2D(GLenum id, GLuint texture);

//=============================================================================
// Sampler
//=============================================================================
struct SamplerInfo final
{
	TextureFilter minFilter{ TextureFilter::Linear };
	TextureFilter magFilter{ TextureFilter::Linear };
	TextureWrap wrapS{ TextureWrap::Repeat };
	TextureWrap wrapT{ TextureWrap::Repeat };
	TextureWrap wrapR{ TextureWrap::Repeat };
	float maxAnisotropy{ 1.0f };
	float minLod{ -1000.0f };
	float maxLod{ 1000.0f };
	float lodBias{ 0.0f };
	bool compareEnabled{ false };
	CompareFunc compareFunc{ CompareFunc::Less };
	float borderColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

GLuint CreateSamplerState(const SamplerInfo& info);

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
		CompareFunc depthFunc{ CompareFunc::Less };
		bool depthMask{ true };
	} depthState;

	struct
	{
		bool enable{ false };
		CompareFunc frontFunc{ CompareFunc::Always };
		CompareFunc backFunc{ CompareFunc::Always };
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