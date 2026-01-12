#pragma once

#include "OGLEnum.h"

namespace OGLContext
{
	void SetClearColor(float red, float green, float blue, float alpha = 1.0f);
	void Clear(bool colorBuffer, bool depthBuffer, bool stencilBuffer);

	void SetRasterizationLinesWidth(float width);
	void SetRasterizationMode(RasterizationMode rasterizationMode);

	void DrawElements(PrimitiveMode primitiveMode, uint32_t indexCount);
	void DrawElementsInstanced(PrimitiveMode primitiveMode, uint32_t indexCount, uint32_t instances);
	void DrawArrays(PrimitiveMode primitiveMode, uint32_t vertexCount);
	void DrawArraysInstanced(PrimitiveMode primitiveMode, uint32_t vertexCount, uint32_t instances);

} // namespace OGLContext