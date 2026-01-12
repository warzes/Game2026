//=============================================================================
// Implementation
//=============================================================================

[[nodiscard]] inline GLenum EnumToValue(PixelFormat format) noexcept
{
	switch (format)
	{
	case PixelFormat::None:           std::unreachable(); break;
	case PixelFormat::Red:            return GL_RED;
	case PixelFormat::Rg:             return GL_RG;
	case PixelFormat::Rgb:            return GL_RGB;
	case PixelFormat::Bgr:            return GL_BGR;
	case PixelFormat::Rgba:           return GL_RGBA;
	case PixelFormat::Bgra:           return GL_BGRA;
	case PixelFormat::Stencil:        return GL_STENCIL_INDEX;
	case PixelFormat::DepthComponent: return GL_DEPTH_COMPONENT;
	case PixelFormat::DepthStencil:   return GL_DEPTH24_STENCIL8;
	default: std::unreachable();
	}
}

[[nodiscard]] inline GLint EnumToValue(InternalFormat format) noexcept
{
	switch (format)
	{
	case InternalFormat::R8:                return GL_R8;
	//case InternalFormat::R8_SNORM:          return GL_R8_SNORM;
	//case InternalFormat::R8UI:              return GL_R8UI;
	//case InternalFormat::R8I:               return GL_R8I;
	//case InternalFormat::R16:               return GL_R16;
	//case InternalFormat::R16_SNORM:         return GL_R16_SNORM;
	//case InternalFormat::R16UI:             return GL_R16UI;
	//case InternalFormat::R16I:              return GL_R16I;
	//case InternalFormat::R16F:              return GL_R16F;
	//case InternalFormat::R32F:              return GL_R32F;
	case InternalFormat::RG8:               return GL_RG8;
	//case InternalFormat::RG8_SNORM:         return GL_RG8_SNORM;
	//case InternalFormat::RG8UI:             return GL_RG8UI;
	//case InternalFormat::RG8I:              return GL_RG8I;
	//case InternalFormat::RG16:              return GL_RG16;
	//case InternalFormat::RG16_SNORM:        return GL_RG16_SNORM;
	//case InternalFormat::RG16UI:            return GL_RG16UI;
	//case InternalFormat::RG16I:             return GL_RG16I;
	//case InternalFormat::RG16F:             return GL_RG16F;
	//case InternalFormat::RG32F:             return GL_RG32F;
	case InternalFormat::RGB8:              return GL_RGB8;
	case InternalFormat::SRGB8:             return GL_SRGB8;
	//case InternalFormat::RGB8_SNORM:        return GL_RGB8_SNORM;
	//case InternalFormat::RGB16F:            return GL_RGB16F;
	//case InternalFormat::RGB32F:            return GL_RGB32F;
	case InternalFormat::RGBA8:             return GL_RGBA8;
	case InternalFormat::SRGB8_ALPHA8:      return GL_SRGB8_ALPHA8;
	//case InternalFormat::RGBA4:             return GL_RGBA4;
	//case InternalFormat::RGB5_A1:           return GL_RGB5_A1;
	//case InternalFormat::RGBA8_SNORM:       return GL_RGBA8_SNORM;
	//case InternalFormat::RGBA16F:           return GL_RGBA16F;
	//case InternalFormat::RGBA32F:           return GL_RGBA32F;
	//case InternalFormat::R11F_G11F_B10F:    return GL_R11F_G11F_B10F;
	//case InternalFormat::DepthComponent16:  return GL_DEPTH_COMPONENT16;
	//case InternalFormat::DepthComponent24:  return GL_DEPTH_COMPONENT24;
	//case InternalFormat::DepthComponent32F: return GL_DEPTH_COMPONENT32F;
	//case InternalFormat::Depth24Stencil8:   return GL_DEPTH24_STENCIL8;
	default: std::unreachable();
	}
}

[[nodiscard]] inline GLenum EnumToValue(PixelType format) noexcept
{
	switch (format)
	{
	case PixelType::UnsignedByte:      return GL_UNSIGNED_BYTE;
	case PixelType::Byte:              return GL_BYTE;
	case PixelType::UnsignedShort:     return GL_UNSIGNED_SHORT;
	case PixelType::Short:             return GL_SHORT;
	case PixelType::UnsignedInt:       return GL_UNSIGNED_INT;
	case PixelType::Int:               return GL_INT;
	case PixelType::Float:             return GL_FLOAT;
	default: std::unreachable();
	}
}

[[nodiscard]] inline GLenum GetGLEnum(PrimitiveMode mode) noexcept
{
	switch (mode)
	{
	case PrimitiveMode::Points:                 return GL_POINTS;
	case PrimitiveMode::Lines:                  return GL_LINES;
	case PrimitiveMode::LineLoop:               return GL_LINE_LOOP;
	case PrimitiveMode::LineStrip:              return GL_LINE_STRIP;
	case PrimitiveMode::Triangles:              return GL_TRIANGLES;
	case PrimitiveMode::TriangleStrip:          return GL_TRIANGLE_STRIP;
	case PrimitiveMode::TriangleFan:            return GL_TRIANGLE_FAN;
	case PrimitiveMode::LinesAdjacency:         return GL_LINES_ADJACENCY;
	case PrimitiveMode::LineStripAdjacency:     return GL_LINE_STRIP_ADJACENCY;
	case PrimitiveMode::TrianglesAdjacency:     return GL_TRIANGLES_ADJACENCY;
	case PrimitiveMode::TriangleStripAdjacency: return GL_TRIANGLE_STRIP_ADJACENCY;
	default: std::unreachable();
	}
}

inline GLenum EnumToValue(RasterizationMode mode) noexcept
{
	switch (mode)
	{
	case RasterizationMode::Point: return GL_POINT;
	case RasterizationMode::Line:  return GL_LINE;
	case RasterizationMode::Fill:  return GL_FILL;
	default: std::unreachable();
	}
}