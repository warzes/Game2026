#pragma once

#include "NanoOpenGL3.h"
#include "OGLBuffer.h"

//=============================================================================
// GPU Buffer
//=============================================================================

struct GPUBuffer final
{
	BufferHandle handle;
	unsigned     size{ 0 };
	GLenum       target{ 0 };
	BufferUsage  usage{ BufferUsage::StaticDraw };
};

inline bool IsValid(const GPUBuffer& resource) { return resource.handle.handle > 0; }

void GPUFree(GPUBuffer& resource);


// TODO: аналогично остальное
// GPUTexture
// GPUSampler
// GPURenderBuffer
// GPUFramebuffer