#include "stdafx.h"
#include "OGLBuffer.h"
//=============================================================================
inline GLenum EnumToValue(BufferUsage mode) noexcept
{
	switch (mode) {
	case BufferUsage::StaticDraw:  return GL_STATIC_DRAW;
	case BufferUsage::DynamicDraw: return GL_DYNAMIC_DRAW;
	case BufferUsage::StreamDraw:  return GL_STREAM_DRAW;
	case BufferUsage::StaticRead:  return GL_STATIC_READ;
	case BufferUsage::DynamicRead: return GL_DYNAMIC_READ;
	case BufferUsage::StreamRead:  return GL_STREAM_READ;
	case BufferUsage::StaticCopy:  return GL_STATIC_COPY;
	case BufferUsage::DynamicCopy: return GL_DYNAMIC_COPY;
	case BufferUsage::StreamCopy:  return GL_STREAM_COPY;
	default: std::unreachable();
	}
}
//=============================================================================
inline GLenum EnumToValue(BufferTarget type) noexcept
{
	switch (type) {
	case BufferTarget::Array:        return GL_ARRAY_BUFFER;
	case BufferTarget::ElementArray: return GL_ELEMENT_ARRAY_BUFFER;
	case BufferTarget::Uniform:      return GL_UNIFORM_BUFFER;
	default: std::unreachable();
	}
}
//=============================================================================
GLuint GetCurrentBuffer(BufferTarget target)
{
	GLenum glTarget = EnumToValue(target);
	GLenum targetBinding{ 0 };
	switch (glTarget)
	{
	case GL_ARRAY_BUFFER:         targetBinding = GL_ARRAY_BUFFER_BINDING; break;
	case GL_ELEMENT_ARRAY_BUFFER: targetBinding = GL_ELEMENT_ARRAY_BUFFER_BINDING; break;
	case GL_UNIFORM_BUFFER:       targetBinding = GL_UNIFORM_BUFFER_BINDING; break;
	default: std::unreachable(); break;
	}
	GLuint currentBuffer{ 0 };
	glGetIntegerv(targetBinding, (GLint*)&currentBuffer);

	return currentBuffer;
}
//=============================================================================
BufferHandle CreateBuffer(BufferTarget target, BufferUsage usage, size_t size, const void* data)
{
	GLuint currentBuffer = GetCurrentBuffer(target);
	GLenum glTarget = EnumToValue(target);

	BufferHandle buffer{};
	glGenBuffers(1, &buffer.handle);
	glBindBuffer(glTarget, buffer.handle);
	glBufferData(glTarget, static_cast<GLsizeiptr>(size), data, glTarget);
	glBindBuffer(glTarget, currentBuffer);

	return buffer;
}
//=============================================================================
void BufferSubData(BufferHandle bufferId, BufferTarget target, GLintptr offset, GLsizeiptr size, const void* data)
{
	GLuint currentBuffer = GetCurrentBuffer(target);
	GLenum glTarget = EnumToValue(target);

	glBindBuffer(glTarget, bufferId.handle);
	glBufferSubData(glTarget, offset, size, data);
	glBindBuffer(glTarget, currentBuffer);
}
//=============================================================================