#pragma once

#include <cstdint>
#include "opengl_types.hpp"

// Хендлы ресурсов OpenGL - легковесные структуры без внутренней логики
struct BufferHandle final {
    GLuint handle{ 0u };
    
    explicit constexpr BufferHandle(GLuint h = 0u) noexcept : handle(h) {}
    constexpr bool operator==(const BufferHandle& other) const noexcept { return handle == other.handle; }
    constexpr bool operator!=(const BufferHandle& other) const noexcept { return handle != other.handle; }
    constexpr bool operator<(const BufferHandle& other) const noexcept { return handle < other.handle; }
    constexpr bool valid() const noexcept { return handle != 0u; }
};

struct TextureHandle final {
    GLuint handle{ 0u };
    
    explicit constexpr TextureHandle(GLuint h = 0u) noexcept : handle(h) {}
    constexpr bool operator==(const TextureHandle& other) const noexcept { return handle == other.handle; }
    constexpr bool operator!=(const TextureHandle& other) const noexcept { return handle != other.handle; }
    constexpr bool operator<(const TextureHandle& other) const noexcept { return handle < other.handle; }
    constexpr bool valid() const noexcept { return handle != 0u; }
};

struct ShaderHandle final {
    GLuint handle{ 0u };
    
    explicit constexpr ShaderHandle(GLuint h = 0u) noexcept : handle(h) {}
    constexpr bool operator==(const ShaderHandle& other) const noexcept { return handle == other.handle; }
    constexpr bool operator!=(const ShaderHandle& other) const noexcept { return handle != other.handle; }
    constexpr bool operator<(const ShaderHandle& other) const noexcept { return handle < other.handle; }
    constexpr bool valid() const noexcept { return handle != 0u; }
};

struct ProgramHandle final {
    GLuint handle{ 0u };
    
    explicit constexpr ProgramHandle(GLuint h = 0u) noexcept : handle(h) {}
    constexpr bool operator==(const ProgramHandle& other) const noexcept { return handle == other.handle; }
    constexpr bool operator!=(const ProgramHandle& other) const noexcept { return handle != other.handle; }
    constexpr bool operator<(const ProgramHandle& other) const noexcept { return handle < other.handle; }
    constexpr bool valid() const noexcept { return handle != 0u; }
};

struct VertexArrayHandle final {
    GLuint handle{ 0u };
    
    explicit constexpr VertexArrayHandle(GLuint h = 0u) noexcept : handle(h) {}
    constexpr bool operator==(const VertexArrayHandle& other) const noexcept { return handle == other.handle; }
    constexpr bool operator!=(const VertexArrayHandle& other) const noexcept { return handle != other.handle; }
    constexpr bool operator<(const VertexArrayHandle& other) const noexcept { return handle < other.handle; }
    constexpr bool valid() const noexcept { return handle != 0u; }
};

struct FramebufferHandle final {
    GLuint handle{ 0u };
    
    explicit constexpr FramebufferHandle(GLuint h = 0u) noexcept : handle(h) {}
    constexpr bool operator==(const FramebufferHandle& other) const noexcept { return handle == other.handle; }
    constexpr bool operator!=(const FramebufferHandle& other) const noexcept { return handle != other.handle; }
    constexpr bool operator<(const FramebufferHandle& other) const noexcept { return handle < other.handle; }
    constexpr bool valid() const noexcept { return handle != 0u; }
};

struct RenderbufferHandle final {
    GLuint handle{ 0u };
    
    explicit constexpr RenderbufferHandle(GLuint h = 0u) noexcept : handle(h) {}
    constexpr bool operator==(const RenderbufferHandle& other) const noexcept { return handle == other.handle; }
    constexpr bool operator!=(const RenderbufferHandle& other) const noexcept { return handle != other.handle; }
    constexpr bool operator<(const RenderbufferHandle& other) const noexcept { return handle < other.handle; }
    constexpr bool valid() const noexcept { return handle != 0u; }
};

struct SamplerHandle final {
    GLuint handle{ 0u };
    
    explicit constexpr SamplerHandle(GLuint h = 0u) noexcept : handle(h) {}
    constexpr bool operator==(const SamplerHandle& other) const noexcept { return handle == other.handle; }
    constexpr bool operator!=(const SamplerHandle& other) const noexcept { return handle != other.handle; }
    constexpr bool operator<(const SamplerHandle& other) const noexcept { return handle < other.handle; }
    constexpr bool valid() const noexcept { return handle != 0u; }
};

struct TransformFeedbackHandle final {
    GLuint handle{ 0u };
    
    explicit constexpr TransformFeedbackHandle(GLuint h = 0u) noexcept : handle(h) {}
    constexpr bool operator==(const TransformFeedbackHandle& other) const noexcept { return handle == other.handle; }
    constexpr bool operator!=(const TransformFeedbackHandle& other) const noexcept { return handle != other.handle; }
    constexpr bool operator<(const TransformFeedbackHandle& other) const noexcept { return handle < other.handle; }
    constexpr bool valid() const noexcept { return handle != 0u; }
};

struct QueryHandle final {
    GLuint handle{ 0u };
    
    explicit constexpr QueryHandle(GLuint h = 0u) noexcept : handle(h) {}
    constexpr bool operator==(const QueryHandle& other) const noexcept { return handle == other.handle; }
    constexpr bool operator!=(const QueryHandle& other) const noexcept { return handle != other.handle; }
    constexpr bool operator<(const QueryHandle& other) const noexcept { return handle < other.handle; }
    constexpr bool valid() const noexcept { return handle != 0u; }
};

struct PipelineHandle final {
    GLuint handle{ 0u };
    
    explicit constexpr PipelineHandle(GLuint h = 0u) noexcept : handle(h) {}
    constexpr bool operator==(const PipelineHandle& other) const noexcept { return handle == other.handle; }
    constexpr bool operator!=(const PipelineHandle& other) const noexcept { return handle != other.handle; }
    constexpr bool operator<(const PipelineHandle& other) const noexcept { return handle < other.handle; }
    constexpr bool valid() const noexcept { return handle != 0u; }
};

// Спецификаторы вершинных атрибутов
struct VertexAttribBinding final {
    GLuint index{ 0u };
    
    explicit constexpr VertexAttribBinding(GLuint idx = 0u) noexcept : index(idx) {}
    constexpr bool operator==(const VertexAttribBinding& other) const noexcept { return index == other.index; }
    constexpr bool operator!=(const VertexAttribBinding& other) const noexcept { return index != other.index; }
    constexpr bool operator<(const VertexAttribBinding& other) const noexcept { return index < other.index; }
};

struct VertexBufferBinding final {
    GLuint index{ 0u };
    
    explicit constexpr VertexBufferBinding(GLuint idx = 0u) noexcept : index(idx) {}
    constexpr bool operator==(const VertexBufferBinding& other) const noexcept { return index == other.index; }
    constexpr bool operator!=(const VertexBufferBinding& other) const noexcept { return index != other.index; }
    constexpr bool operator<(const VertexBufferBinding& other) const noexcept { return index < other.index; }
};