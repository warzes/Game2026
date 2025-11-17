#pragma once

#include <cstdint>

// Базовые типы OpenGL
using GLenum = unsigned int;
using GLuint = unsigned int;
using GLint = int;
using GLsizei = int;
using GLfloat = float;
using GLdouble = double;
using GLboolean = unsigned char;
using GLbitfield = unsigned int;
using GLubyte = unsigned char;
using GLushort = unsigned short;
using GLbyte = signed char;
using GLshort = signed short;

// Перечисления для OpenGL 4.6
enum class ComparisonFunc : uint8_t {
    Never = 0x0200,      // GL_NEVER
    Less = 0x0201,       // GL_LESS
    Equal = 0x0202,      // GL_EQUAL
    LessEqual = 0x0203,  // GL_LEQUAL
    Greater = 0x0204,    // GL_GREATER
    NotEqual = 0x0205,   // GL_NOTEQUAL
    GreaterEqual = 0x0206, // GL_GEQUAL
    Always = 0x0207      // GL_ALWAYS
};

enum class BufferType : GLenum {
    ArrayBuffer = 0x8892,        // GL_ARRAY_BUFFER
    ElementArrayBuffer = 0x8893, // GL_ELEMENT_ARRAY_BUFFER
    UniformBuffer = 0x8A11,      // GL_UNIFORM_BUFFER
    ShaderStorageBuffer = 0x90D2, // GL_SHADER_STORAGE_BUFFER
    TextureBuffer = 0x8C2A,      // GL_TEXTURE_BUFFER
    TransformFeedbackBuffer = 0x8C8E, // GL_TRANSFORM_FEEDBACK_BUFFER
    CopyReadBuffer = 0x8F36,     // GL_COPY_READ_BUFFER
    CopyWriteBuffer = 0x8F37,    // GL_COPY_WRITE_BUFFER
    DispatchIndirectBuffer = 0x90EE, // GL_DISPATCH_INDIRECT_BUFFER
    DrawIndirectBuffer = 0x8F3F, // GL_DRAW_INDIRECT_BUFFER
    PixelPackBuffer = 0x88EB,    // GL_PIXEL_PACK_BUFFER
    PixelUnpackBuffer = 0x88EC,  // GL_PIXEL_UNPACK_BUFFER
    QueryBuffer = 0x9192         // GL_QUERY_BUFFER
};

enum class BufferUsage : GLenum {
    StreamDraw = 0x88E0,    // GL_STREAM_DRAW
    StreamRead = 0x88E1,    // GL_STREAM_READ
    StreamCopy = 0x88E2,    // GL_STREAM_COPY
    StaticDraw = 0x88E4,    // GL_STATIC_DRAW
    StaticRead = 0x88E5,    // GL_STATIC_READ
    StaticCopy = 0x88E6,    // GL_STATIC_COPY
    DynamicDraw = 0x88E8,   // GL_DYNAMIC_DRAW
    DynamicRead = 0x88E9,   // GL_DYNAMIC_READ
    DynamicCopy = 0x88EA    // GL_DYNAMIC_COPY
};

enum class ShaderType : GLenum {
    VertexShader = 0x8B31,        // GL_VERTEX_SHADER
    FragmentShader = 0x8B30,      // GL_FRAGMENT_SHADER
    GeometryShader = 0x8DD9,      // GL_GEOMETRY_SHADER
    TessControlShader = 0x8E88,   // GL_TESS_CONTROL_SHADER
    TessEvaluationShader = 0x8E87, // GL_TESS_EVALUATION_SHADER
    ComputeShader = 0x91B9        // GL_COMPUTE_SHADER
};

enum class TextureTarget : GLenum {
    Texture1D = 0x0DE0,           // GL_TEXTURE_1D
    Texture2D = 0x0DE1,           // GL_TEXTURE_2D
    Texture3D = 0x806F,           // GL_TEXTURE_3D
    Texture1DArray = 0x8C18,      // GL_TEXTURE_1D_ARRAY
    Texture2DArray = 0x8C1A,      // GL_TEXTURE_2D_ARRAY
    TextureRectangle = 0x84F5,    // GL_TEXTURE_RECTANGLE
    TextureCubeMap = 0x8513,      // GL_TEXTURE_CUBE_MAP
    TextureBuffer = 0x8C2A,       // GL_TEXTURE_BUFFER
    Texture2DMultisample = 0x9100, // GL_TEXTURE_2D_MULTISAMPLE
    Texture2DMultisampleArray = 0x9102 // GL_TEXTURE_2D_MULTISAMPLE_ARRAY
};

enum class TextureInternalFormat : GLenum {
    R8 = 0x8229,           // GL_R8
    R16 = 0x822A,          // GL_R16
    RG8 = 0x822B,          // GL_RG8
    RG16 = 0x822C,         // GL_RG16
    R3_G3_B2 = 0x2A10,     // GL_R3_G3_B2
    RGB4 = 0x804F,         // GL_RGB4
    RGB5 = 0x8050,         // GL_RGB5
    RGB8 = 0x8051,         // GL_RGB8
    RGB10 = 0x8052,        // GL_RGB10
    RGB12 = 0x8053,        // GL_RGB12
    RGB16 = 0x8054,        // GL_RGB16
    RGBA2 = 0x8055,        // GL_RGBA2
    RGBA4 = 0x8056,        // GL_RGBA4
    RGB5_A1 = 0x8057,      // GL_RGB5_A1
    RGBA8 = 0x8058,        // GL_RGBA8
    RGB10_A2 = 0x8059,     // GL_RGB10_A2
    RGBA12 = 0x805A,       // GL_RGBA12
    RGBA16 = 0x805B,       // GL_RGBA16
    SRGB8 = 0x8C41,        // GL_SRGB8
    SRGB8_ALPHA8 = 0x8C43, // GL_SRGB8_ALPHA8
    R16F = 0x822D,         // GL_R16F
    RG16F = 0x822F,        // GL_RG16F
    RGB16F = 0x881B,       // GL_RGB16F
    RGBA16F = 0x881A,      // GL_RGBA16F
    R32F = 0x822E,         // GL_R32F
    RG32F = 0x8230,        // GL_RG32F
    RGB32F = 0x8815,       // GL_RGB32F
    RGBA32F = 0x8814,      // GL_RGBA32F
    R11F_G11F_B10F = 0x8C3A, // GL_R11F_G11F_B10F
    RGB9_E5 = 0x8C3D,      // GL_RGB9_E5
    R8I = 0x8231,          // GL_R8I
    R8UI = 0x8232,         // GL_R8UI
    R16I = 0x8233,         // GL_R16I
    R16UI = 0x8234,        // GL_R16UI
    R32I = 0x8235,         // GL_R32I
    R32UI = 0x8236,        // GL_R32UI
    RG8I = 0x8237,         // GL_RG8I
    RG8UI = 0x8238,        // GL_RG8UI
    RG16I = 0x8239,        // GL_RG16I
    RG16UI = 0x823A,       // GL_RG16UI
    RG32I = 0x823B,        // GL_RG32I
    RG32UI = 0x823C,       // GL_RG32UI
    RGB8I = 0x8D8F,        // GL_RGB8I
    RGB8UI = 0x8D7D,       // GL_RGB8UI
    RGB16I = 0x8D89,       // GL_RGB16I
    RGB16UI = 0x8D77,      // GL_RGB16UI
    RGB32I = 0x8D83,       // GL_RGB32I
    RGB32UI = 0x8D71,      // GL_RGB32UI
    RGBA8I = 0x8D8E,       // GL_RGBA8I
    RGBA8UI = 0x8D7C,      // GL_RGBA8UI
    RGBA16I = 0x8D88,      // GL_RGBA16I
    RGBA16UI = 0x8D76,     // GL_RGBA16UI
    RGBA32I = 0x8D82,      // GL_RGBA32I
    RGBA32UI = 0x8D70,     // GL_RGBA32UI
    DepthComponent16 = 0x81A5, // GL_DEPTH_COMPONENT16
    DepthComponent24 = 0x81A6, // GL_DEPTH_COMPONENT24
    DepthComponent32 = 0x81A7, // GL_DEPTH_COMPONENT32
    DepthComponent32F = 0x8CAC, // GL_DEPTH_COMPONENT32F
    Depth24Stencil8 = 0x88F0,  // GL_DEPTH24_STENCIL8
    Depth32FStencil8 = 0x8CAD  // GL_DEPTH32F_STENCIL8
};

enum class TextureFormat : GLenum {
    Red = 0x1903,          // GL_RED
    RG = 0x8227,           // GL_RG
    RGB = 0x1907,          // GL_RGB
    BGR = 0x80E0,          // GL_BGR
    RGBA = 0x1908,         // GL_RGBA
    BGRA = 0x80E1,         // GL_BGRA
    RedInteger = 0x8D94,   // GL_RED_INTEGER
    RGInteger = 0x8228,    // GL_RG_INTEGER
    RGBInteger = 0x8D98,   // GL_RGB_INTEGER
    BGRInteger = 0x8D9A,   // GL_BGR_INTEGER
    RGBAInteger = 0x8D99,  // GL_RGBA_INTEGER
    BGRAInteger = 0x8D9B,  // GL_BGRA_INTEGER
    StencilIndex = 0x1901, // GL_STENCIL_INDEX
    DepthComponent = 0x1902, // GL_DEPTH_COMPONENT
    DepthStencil = 0x84F9  // GL_DEPTH_STENCIL
};

enum class TextureDataType : GLenum {
    UnsignedByte = 0x1401,     // GL_UNSIGNED_BYTE
    Byte = 0x1400,             // GL_BYTE
    UnsignedShort = 0x1403,    // GL_UNSIGNED_SHORT
    Short = 0x1402,            // GL_SHORT
    UnsignedInt = 0x1405,      // GL_UNSIGNED_INT
    Int = 0x1404,              // GL_INT
    Float = 0x1406,            // GL_FLOAT
    UnsignedByte332 = 0x8032, // GL_UNSIGNED_BYTE_3_3_2
    UnsignedByte233Rev = 0x8362, // GL_UNSIGNED_BYTE_2_3_3_REV
    UnsignedShort565 = 0x8363, // GL_UNSIGNED_SHORT_5_6_5
    UnsignedShort565Rev = 0x8364, // GL_UNSIGNED_SHORT_5_6_5_REV
    UnsignedShort444 = 0x8033, // GL_UNSIGNED_SHORT_4_4_4_4
    UnsignedShort4444Rev = 0x8365, // GL_UNSIGNED_SHORT_4_4_4_4_REV
    UnsignedShort5551 = 0x8034, // GL_UNSIGNED_SHORT_5_5_5_1
    UnsignedShort1555Rev = 0x8366, // GL_UNSIGNED_SHORT_1_5_5_5_REV
    UnsignedInt8888 = 0x8035, // GL_UNSIGNED_INT_8_8_8_8
    UnsignedInt8888Rev = 0x8367, // GL_UNSIGNED_INT_8_8_8_8_REV
    UnsignedInt1010102 = 0x8036, // GL_UNSIGNED_INT_10_10_10_2
    UnsignedInt2101010Rev = 0x8368, // GL_UNSIGNED_INT_2_10_10_REV
    UnsignedInt248 = 0x84FA,   // GL_UNSIGNED_INT_24_8
    UnsignedInt10F11F11FRev = 0x8C3B, // GL_UNSIGNED_INT_10F_11F_11F_REV
    UnsignedInt5999Rev = 0x8C3E, // GL_UNSIGNED_INT_5_9_9_9_REV
    HalfFloat = 0x140B,        // GL_HALF_FLOAT
    Double = 0x140A            // GL_DOUBLE
};

enum class WrapMode : GLenum {
    Repeat = 0x2901,              // GL_REPEAT
    MirroredRepeat = 0x8370,      // GL_MIRRORED_REPEAT
    ClampToEdge = 0x812F,         // GL_CLAMP_TO_EDGE
    ClampToBorder = 0x812D,       // GL_CLAMP_TO_BORDER
    MirrorClampToEdge = 0x8743    // GL_MIRROR_CLAMP_TO_EDGE
};

enum class FilterMode : GLenum {
    Nearest = 0x2600,             // GL_NEAREST
    Linear = 0x2601,              // GL_LINEAR
    NearestMipmapNearest = 0x2700, // GL_NEAREST_MIPMAP_NEAREST
    LinearMipmapNearest = 0x2701, // GL_LINEAR_MIPMAP_NEAREST
    NearestMipmapLinear = 0x2702, // GL_NEAREST_MIPMAP_LINEAR
    LinearMipmapLinear = 0x2703  // GL_LINEAR_MIPMAP_LINEAR
};

enum class PrimitiveType : GLenum {
    Points = 0x0000,              // GL_POINTS
    Lines = 0x0001,               // GL_LINES
    LineLoop = 0x0002,            // GL_LINE_LOOP
    LineStrip = 0x0003,           // GL_LINE_STRIP
    Triangles = 0x0004,           // GL_TRIANGLES
    TriangleStrip = 0x0005,       // GL_TRIANGLE_STRIP
    TriangleFan = 0x0006,         // GL_TRIANGLE_FAN
    LinesAdjacency = 0x000A,      // GL_LINES_ADJACENCY
    LineStripAdjacency = 0x000B,  // GL_LINE_STRIP_ADJACENCY
    TrianglesAdjacency = 0x000C, // GL_TRIANGLES_ADJACENCY
    TriangleStripAdjacency = 0x000D, // GL_TRIANGLE_STRIP_ADJACENCY
    Patches = 0x000E              // GL_PATCHES
};

enum class CullFaceMode : GLenum {
    Front = 0x0404,               // GL_FRONT
    Back = 0x0405,                // GL_BACK
    FrontAndBack = 0x0408         // GL_FRONT_AND_BACK
};

enum class FrontFaceMode : GLenum {
    CW = 0x0900,                  // GL_CW
    CCW = 0x0901                  // GL_CCW
};

enum class PolygonMode : GLenum {
    Point = 0x1B00,               // GL_POINT
    Line = 0x1B01,                // GL_LINE
    Fill = 0x1B02                 // GL_FILL
};

enum class BlendEquation : GLenum {
    FuncAdd = 0x8006,             // GL_FUNC_ADD
    FuncSubtract = 0x800A,        // GL_FUNC_SUBTRACT
    FuncReverseSubtract = 0x800B, // GL_FUNC_REVERSE_SUBTRACT
    Min = 0x8007,                 // GL_MIN
    Max = 0x8008                  // GL_MAX
};

enum class BlendFactor : GLenum {
    Zero = 0,                     // GL_ZERO
    One = 1,                      // GL_ONE
    SrcColor = 0x0300,            // GL_SRC_COLOR
    OneMinusSrcColor = 0x0301,    // GL_ONE_MINUS_SRC_COLOR
    DstColor = 0x0306,            // GL_DST_COLOR
    OneMinusDstColor = 0x0307,    // GL_ONE_MINUS_DST_COLOR
    SrcAlpha = 0x0302,            // GL_SRC_ALPHA
    OneMinusSrcAlpha = 0x0303,    // GL_ONE_MINUS_SRC_ALPHA
    DstAlpha = 0x0304,            // GL_DST_ALPHA
    OneMinusDstAlpha = 0x0305,    // GL_ONE_MINUS_DST_ALPHA
    ConstantColor = 0x8001,       // GL_CONSTANT_COLOR
    OneMinusConstantColor = 0x8002, // GL_ONE_MINUS_CONSTANT_COLOR
    ConstantAlpha = 0x8003,       // GL_CONSTANT_ALPHA
    OneMinusConstantAlpha = 0x8004, // GL_ONE_MINUS_CONSTANT_ALPHA
    SrcAlphaSaturate = 0x0308,    // GL_SRC_ALPHA_SATURATE
    Src1Color = 0x88F9,           // GL_SRC1_COLOR
    OneMinusSrc1Color = 0x88FA,   // GL_ONE_MINUS_SRC1_COLOR
    Src1Alpha = 0x8589,           // GL_SRC1_ALPHA
    OneMinusSrc1Alpha = 0x88FB    // GL_ONE_MINUS_SRC1_ALPHA
};

enum class DataType : GLenum {
    Byte = 0x1400,                // GL_BYTE
    UnsignedByte = 0x1401,        // GL_UNSIGNED_BYTE
    Short = 0x1402,               // GL_SHORT
    UnsignedShort = 0x1403,       // GL_UNSIGNED_SHORT
    Int = 0x1404,                 // GL_INT
    UnsignedInt = 0x1405,         // GL_UNSIGNED_INT
    Float = 0x1406,               // GL_FLOAT
    Double = 0x140A,              // GL_DOUBLE
    HalfFloat = 0x140B,           // GL_HALF_FLOAT
    Int2101010 = 0x8D9F,          // GL_INT_2_10_10_10_REV
    UnsignedInt21010 = 0x8368, // GL_UNSIGNED_INT_2_10_10_10_REV
    UnsignedInt10F11F11FRev = 0x8C3B, // GL_UNSIGNED_INT_10F_11F_11F_REV
    UnsignedInt5999Rev = 0x8C3E   // GL_UNSIGNED_INT_5_9_9_9_REV
};

enum class ClearBufferMask : GLbitfield {
    ColorBufferBit = 0x00004000,   // GL_COLOR_BUFFER_BIT
    DepthBufferBit = 0x00000100,   // GL_DEPTH_BUFFER_BIT
    StencilBufferBit = 0x00000400  // GL_STENCIL_BUFFER_BIT
};

enum class FramebufferTarget : GLenum {
    ReadFramebuffer = 0x8CA8,     // GL_READ_FRAMEBUFFER
    DrawFramebuffer = 0x8CA9,     // GL_DRAW_FRAMEBUFFER
    Framebuffer = 0x8D40          // GL_FRAMEBUFFER (for both read and draw)
};

enum class FramebufferAttachment : GLenum {
    ColorAttachment0 = 0x8CE0,    // GL_COLOR_ATTACHMENT0
    ColorAttachment1 = 0x8CE1,    // GL_COLOR_ATTACHMENT1
    ColorAttachment2 = 0x8CE2,    // GL_COLOR_ATTACHMENT2
    ColorAttachment3 = 0x8CE3,    // GL_COLOR_ATTACHMENT3
    ColorAttachment4 = 0x8CE4,    // GL_COLOR_ATTACHMENT4
    ColorAttachment5 = 0x8CE5,    // GL_COLOR_ATTACHMENT5
    ColorAttachment6 = 0x8CE6,    // GL_COLOR_ATTACHMENT6
    ColorAttachment7 = 0x8CE7,    // GL_COLOR_ATTACHMENT7
    ColorAttachment8 = 0x8CE8,    // GL_COLOR_ATTACHMENT8
    ColorAttachment9 = 0x8CE9,    // GL_COLOR_ATTACHMENT9
    ColorAttachment10 = 0x8CEA,   // GL_COLOR_ATTACHMENT10
    ColorAttachment11 = 0x8CEB,   // GL_COLOR_ATTACHMENT11
    ColorAttachment12 = 0x8CEC,   // GL_COLOR_ATTACHMENT12
    ColorAttachment13 = 0x8CED,   // GL_COLOR_ATTACHMENT13
    ColorAttachment14 = 0x8CEE,   // GL_COLOR_ATTACHMENT14
    ColorAttachment15 = 0x8CEF,   // GL_COLOR_ATTACHMENT15
    DepthAttachment = 0x8D00,     // GL_DEPTH_ATTACHMENT
    StencilAttachment = 0x8D20,   // GL_STENCIL_ATTACHMENT
    DepthStencilAttachment = 0x821A // GL_DEPTH_STENCIL_ATTACHMENT
};

enum class FramebufferStatus : GLenum {
    FramebufferComplete = 0x8CD5,           // GL_FRAMEBUFFER_COMPLETE
    FramebufferUndefined = 0x8219,          // GL_FRAMEBUFFER_UNDEFINED
    FramebufferIncompleteAttachment = 0x8CD6, // GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
    FramebufferIncompleteMissingAttachment = 0x8CD7, // GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
    FramebufferIncompleteDrawBuffer = 0x8CDB, // GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
    FramebufferIncompleteReadBuffer = 0x8CDC, // GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
    FramebufferUnsupported = 0x8CDD,        // GL_FRAMEBUFFER_UNSUPPORTED
    FramebufferIncompleteMultisample = 0x8D56, // GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
    FramebufferIncompleteLayerTargets = 0x8DA8 // GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
};

enum class DrawBufferMode : GLenum {
    None = 0,                     // GL_NONE
    FrontLeft = 0x0400,           // GL_FRONT_LEFT
    FrontRight = 0x0401,          // GL_FRONT_RIGHT
    BackLeft = 0x0402,            // GL_BACK_LEFT
    BackRight = 0x0403,           // GL_BACK_RIGHT
    Front = 0x0404,               // GL_FRONT
    Back = 0x0405,                // GL_BACK
    Left = 0x0406,                // GL_LEFT
    Right = 0x0407,               // GL_RIGHT
    FrontAndBack = 0x0408,        // GL_FRONT_AND_BACK
    ColorAttachment0 = 0x8CE0,    // GL_COLOR_ATTACHMENT0
    ColorAttachment1 = 0x8CE1,    // GL_COLOR_ATTACHMENT1
    ColorAttachment2 = 0x8CE2,    // GL_COLOR_ATTACHMENT2
    ColorAttachment3 = 0x8CE3,    // GL_COLOR_ATTACHMENT3
    ColorAttachment4 = 0x8CE4,    // GL_COLOR_ATTACHMENT4
    ColorAttachment5 = 0x8CE5,    // GL_COLOR_ATTACHMENT5
    ColorAttachment6 = 0x8CE6,    // GL_COLOR_ATTACHMENT6
    ColorAttachment7 = 0x8CE7,    // GL_COLOR_ATTACHMENT7
    ColorAttachment8 = 0x8CE8,    // GL_COLOR_ATTACHMENT8
    ColorAttachment9 = 0x8CE9,    // GL_COLOR_ATTACHMENT9
    ColorAttachment10 = 0x8CEA,   // GL_COLOR_ATTACHMENT10
    ColorAttachment11 = 0x8CEB,   // GL_COLOR_ATTACHMENT11
    ColorAttachment12 = 0x8CEC,   // GL_COLOR_ATTACHMENT12
    ColorAttachment13 = 0x8CED,   // GL_COLOR_ATTACHMENT13
    ColorAttachment14 = 0x8CEE,   // GL_COLOR_ATTACHMENT14
    ColorAttachment15 = 0x8CEF    // GL_COLOR_ATTACHMENT15
};

enum class EnableCap : GLenum {
    Blend = 0x0BE2,               // GL_BLEND
    CullFace = 0x0B44,            // GL_CULL_FACE
    DepthTest = 0x0B71,           // GL_DEPTH_TEST
    Dither = 0x0BD0,              // GL_DITHER
    PolygonOffsetFill = 0x8037,   // GL_POLYGON_OFFSET_FILL
    PolygonOffsetLine = 0x2A02,   // GL_POLYGON_OFFSET_LINE
    PolygonOffsetPoint = 0x2A01,  // GL_POLYGON_OFFSET_POINT
    PolygonSmooth = 0x0B41,       // GL_POLYGON_SMOOTH
    SampleAlphaToCoverage = 0x809E, // GL_SAMPLE_ALPHA_TO_COVERAGE
    SampleCoverage = 0x80A0,      // GL_SAMPLE_COVERAGE
    ScissorTest = 0x0C11,         // GL_SCISSOR_TEST
    StencilTest = 0x0B90,         // GL_STENCIL_TEST
    FramebufferSrgb = 0x8DB9,     // GL_FRAMEBUFFER_SRGB
    ProgramPointSize = 0x8642,    // GL_PROGRAM_POINT_SIZE
    RasterizerDiscard = 0x8C89,   // GL_RASTERIZER_DISCARD
    SampleShading = 0x8C36,       // GL_SAMPLE_SHADING
    PrimitiveRestart = 0x8F9D     // GL_PRIMITIVE_RESTART
};

enum class ErrorCode : GLenum {
    NoError = 0,                  // GL_NO_ERROR
    InvalidEnum = 0x0500,         // GL_INVALID_ENUM
    InvalidValue = 0x0501,        // GL_INVALID_VALUE
    InvalidOperation = 0x0502,    // GL_INVALID_OPERATION
    OutOfMemory = 0x0505,         // GL_OUT_OF_MEMORY
    InvalidFramebufferOperation = 0x0506 // GL_INVALID_FRAMEBUFFER_OPERATION
};

enum class SyncCondition : GLenum {
    SyncGpuCommandsComplete = 0x9117 // GL_SYNC_GPU_COMMANDS_COMPLETE
};

enum class SyncStatus : GLenum {
    Signaled = 0x9118,            // GL_SIGNALED
    Unsignaled = 0x9119           // GL_UNSIGNALED
};

enum class SyncFlushFlag : GLbitfield {
    SyncFlushCommandsBit = 0x0000001 // GL_SYNC_FLUSH_COMMANDS_BIT
};

enum class VertexAttribProperty : GLenum {
    VertexAttribArrayEnabled = 0x8622, // GL_VERTEX_ATTRIB_ARRAY_ENABLED
    VertexAttribArraySize = 0x8623, // GL_VERTEX_ATTRIB_ARRAY_SIZE
    VertexAttribArrayStride = 0x8624, // GL_VERTEX_ATTRIB_ARRAY_STRIDE
    VertexAttribArrayType = 0x8625, // GL_VERTEX_ATTRIB_ARRAY_TYPE
    VertexAttribArrayNormalized = 0x886A, // GL_VERTEX_ATTRIB_ARRAY_NORMALIZED
    VertexAttribArrayInteger = 0x88FD, // GL_VERTEX_ATTRIB_ARRAY_INTEGER
    VertexAttribArrayDivisor = 0x88FE, // GL_VERTEX_ATTRIB_ARRAY_DIVISOR
    CurrentVertexAttrib = 0x8626, // GL_CURRENT_VERTEX_ATTRIB
    VertexAttribArrayPointer = 0x8645 // GL_VERTEX_ATTRIB_ARRAY_POINTER
};