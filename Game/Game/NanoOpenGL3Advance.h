#pragma once

#include "NanoOpenGL3.h"

//=============================================================================
// Uniform
//=============================================================================

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
// GPU Buffer
//=============================================================================

struct GPUBuffer final
{
	BufferHandle handle;
	unsigned     size{ 0 };
	GLenum       target{ 0 };
	BufferUsage  usage{ BufferUsage::Static };
};

inline bool IsValid(const GPUBuffer& resource) { return resource.handle.handle > 0; }

void GPUFree(GPUBuffer& resource);


// TODO: аналогично остальное
// GPUTexture
// GPUSampler
// GPURenderBuffer
// GPUFramebuffer