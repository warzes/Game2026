#pragma once

// core.h
namespace OpenGLWrapper
{
	enum class ComparisonFunc : uint8_t
	{
		Never = GL_NEVER,
		Less = GL_LESS,
		Equal = GL_EQUAL,
		LessEqual = GL_LEQUAL,
		Greater = GL_GREATER,
		NotEqual = GL_NOTEQUAL,
		GreaterEqual = GL_GEQUAL,
		Always = GL_ALWAYS
	};

	enum class BufferType : uint8_t
	{
		ArrayBuffer = GL_ARRAY_BUFFER,
		ElementArrayBuffer = GL_ELEMENT_ARRAY_BUFFER,
		UniformBuffer = GL_UNIFORM_BUFFER,
	};

	enum class TextureTarget : uint8_t
	{
		Texture1D = GL_TEXTURE_1D,
		Texture2D = GL_TEXTURE_2D,
		TextureCubeMap = GL_TEXTURE_CUBE_MAP,
		Texture3D = GL_TEXTURE_3D
	};

	enum class DataType : uint8_t
	{
		Float = GL_FLOAT,
		Int = GL_INT,
		UnsignedInt = GL_UNSIGNED_INT,
		UnsignedByte = GL_UNSIGNED_BYTE
	};

	enum class PrimitiveType : uint8_t
	{
		Triangles = GL_TRIANGLES,
		TriangleStrip = GL_TRIANGLE_STRIP,
		Lines = GL_LINES,
		LineStrip = GL_LINE_STRIP,
		Points = GL_POINTS
	};

	enum class BlendFactor : uint8_t
	{
		Zero = GL_ZERO,
		One = GL_ONE,
		SrcColor = GL_SRC_COLOR,
		OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
		DstColor = GL_DST_COLOR,
		OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
		SrcAlpha = GL_SRC_ALPHA,
		OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
		DstAlpha = GL_DST_ALPHA,
		OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA
	};

	enum class CullMode : uint8_t
	{
		None = GL_NONE,
		Front = GL_FRONT,
		Back = GL_BACK,
		FrontAndBack = GL_FRONT_AND_BACK
	};

	enum class FrontFace : uint8_t
	{
		Clockwise = GL_CW,
		CounterClockwise = GL_CCW
	};

	enum class PolygonMode : uint8_t
	{
		Fill = GL_FILL,
		Line = GL_LINE,
		Point = GL_POINT
	};

	enum class StencilOp : uint8_t
	{
		Keep = GL_KEEP,
		Zero = GL_ZERO,
		Replace = GL_REPLACE,
		Increment = GL_INCR,
		Decrement = GL_DECR,
		Invert = GL_INVERT,
		IncrementWrap = GL_INCR_WRAP,
		DecrementWrap = GL_DECR_WRAP
	};

	enum class TextureWrap : uint8_t
	{
		Repeat = GL_REPEAT,
		MirroredRepeat = GL_MIRRORED_REPEAT,
		ClampToEdge = GL_CLAMP_TO_EDGE,
		ClampToBorder = GL_CLAMP_TO_BORDER
	};

	enum class TextureFilter : uint8_t
	{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR,
		NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
		LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
		NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
		LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
	};

	enum class AttachmentPoint : uint8_t
	{
		Color0 = GL_COLOR_ATTACHMENT0,
		Color1 = GL_COLOR_ATTACHMENT1,
		Color2 = GL_COLOR_ATTACHMENT2,
		Color3 = GL_COLOR_ATTACHMENT3,
		Depth = GL_DEPTH_ATTACHMENT,
		Stencil = GL_STENCIL_ATTACHMENT,
		DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT
	};

	enum class DrawBuffer : uint8_t
	{
		None = GL_NONE,
		Back = GL_BACK,
		Front = GL_FRONT,
		Color0 = GL_COLOR_ATTACHMENT0,
		Color1 = GL_COLOR_ATTACHMENT1,
		Color2 = GL_COLOR_ATTACHMENT2,
		Color3 = GL_COLOR_ATTACHMENT3
	};

	enum class BufferUsage : uint8_t
	{
		StaticDraw = GL_STATIC_DRAW,
		DynamicDraw = GL_DYNAMIC_DRAW,
		StreamDraw = GL_STREAM_DRAW,
		StaticRead = GL_STATIC_READ,
		DynamicRead = GL_DYNAMIC_READ,
		StreamRead = GL_STREAM_READ,
		StaticCopy = GL_STATIC_COPY,
		DynamicCopy = GL_DYNAMIC_COPY,
		StreamCopy = GL_STREAM_COPY
	};

	enum class ShaderType : uint8_t
	{
		Vertex = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER,
		Geometry = GL_GEOMETRY_SHADER,
	};

	enum class UniformType : uint8_t
	{
		Float,
		FloatVec2,
		FloatVec3,
		FloatVec4,
		Int,
		IntVec2,
		IntVec3,
		IntVec4,
		Bool,
		BoolVec2,
		BoolVec3,
		BoolVec4,
		FloatMat2,
		FloatMat3,
		FloatMat4,
		Sampler2D,
		SamplerCube
	};

	enum class TextureFormat : uint8_t
	{
		RGBA8,
		RGB8,
		RGBA32F,
		RGB32F,
		RGBA16F,
		RGB16F,
		RG32F,
		R32F,
		Depth,
		DepthStencil,
		Stencil,
		SRGB8,
		SRGB8_ALPHA8,
		RGB565,
		RGBA4,
		RGB10_A2,
		RG8,
		R8,
		RG16F,
		R16F,
		RG32I,
		R32I,
		RG32UI,
		R32UI
	};

	enum class TextureParameterName : uint8_t
	{
		MinFilter,
		MagFilter,
		WrapS,
		WrapT,
		WrapR,
		MaxAnisotropy,
		MinLod,
		MaxLod,
		LodBias,
		CompareMode,
		CompareFunc,
		BorderColor,
		SwizzleR,
		SwizzleG,
		SwizzleB,
		SwizzleA,
		BaseLevel,
		MaxLevel,
		SwizzleRGBA
	};

	enum class ClearFlags : uint8_t
	{
		Color = 0x0001,
		Depth = 0x0002,
		Stencil = 0x0004,
		All = 0x0007
	};

	enum class MemoryBarrierMask : uint32_t
	{
		VertexAttribArray = 0x0001,
		ElementArray = 0x0002,
		Uniform = 0x0004,
		TextureFetch = 0x0008,
		ShaderImageAccess = 0x0010,
		Command = 0x0020,
		PixelBuffer = 0x0040,
		TextureUpdate = 0x0080,
		BufferUpdate = 0x0100,
		Framebuffer = 0x0200,
		TransformFeedback = 0x0400,
		AtomicCounter = 0x0800,
		All = 0xFFFFFFFF
	};

	enum class VertexAttribType : uint8_t
	{
		Float,
		FloatVec2,
		FloatVec3,
		FloatVec4,
		Int,
		IntVec2,
		IntVec3,
		IntVec4,
		Bool,
		BoolVec2,
		BoolVec3,
		BoolVec4,
		FloatMat2,
		FloatMat3,
		FloatMat4
	};

	enum class LogicOp : uint8_t
	{
		Clear = GL_CLEAR,
		And = GL_AND,
		AndReverse = GL_AND_REVERSE,
		Copy = GL_COPY,
		AndInverted = GL_AND_INVERTED,
		NoOp = GL_NOOP,
		Xor = GL_XOR,
		Or = GL_OR,
		Nor = GL_NOR,
		Equiv = GL_EQUIV,
		Invert = GL_INVERT,
		OrReverse = GL_OR_REVERSE,
		CopyInverted = GL_COPY_INVERTED,
		OrInverted = GL_OR_INVERTED,
		Nand = GL_NAND,
		Set = GL_SET
	};

	// Хендлы ресурсов
	struct BufferHandle final
	{
		GLuint handle{ 0u };
	};

	struct ShaderHandle final
	{
		GLuint handle{ 0u };
	};

	struct ProgramHandle final
	{
		GLuint handle{ 0u };
	};

	struct TextureHandle final
	{
		GLuint handle{ 0u };
	};

	struct SamplerHandle final
	{
		GLuint handle{ 0u };
	};

	struct RenderbufferHandle final
	{
		GLuint handle{ 0u };
	};

	struct FramebufferHandle final
	{
		GLuint handle{ 0u };
	};

	struct VertexArrayHandle final
	{
		GLuint handle{ 0u };
	};

	// Структуры состояния
	struct DepthState
	{
		bool enabled{ true };
		ComparisonFunc func{ ComparisonFunc::Less };
		bool writeMask{ true };
	};

	struct StencilState
	{
		bool enabled{ false };
		StencilOp stencilFailOp{ StencilOp::Keep };
		StencilOp depthFailOp{ StencilOp::Keep };
		StencilOp passOp{ StencilOp::Keep };
		ComparisonFunc func{ ComparisonFunc::Always };
		uint32_t reference{ 0u };
		uint32_t mask{ 0xFFFFFFFFu };
	};

	struct BlendState
	{
		bool enabled{ false };
		BlendFactor srcColor{ BlendFactor::SrcAlpha };
		BlendFactor dstColor{ BlendFactor::OneMinusSrcAlpha };
		BlendFactor srcAlpha{ BlendFactor::One };
		BlendFactor dstAlpha{ BlendFactor::OneMinusSrcAlpha };
	};

	struct RasterizerState
	{
		CullMode cullMode{ CullMode::Back };
		FrontFace frontFace{ FrontFace::CounterClockwise };
		PolygonMode polygonMode{ PolygonMode::Fill };
		float lineWidth{ 1.0f };
	};
} // namespace OpenGLWrapper

// functions.h
namespace OpenGLWrapper
{

} // namespace OpenGLWrapper