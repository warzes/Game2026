#pragma once

#include "OGLEnum.h"

namespace OGLContext
{
	void SetClearColor(float red, float green, float blue, float alpha = 1.0f);
	void Clear(bool colorBuffer, bool depthBuffer, bool stencilBuffer = false);

	void SetRasterizationLinesWidth(float width);
	void SetRasterizationMode(RasterizationMode rasterizationMode);

	void SetStencilAlgorithm(ComparisonFunc algorithm, int32_t reference, uint32_t mask);
	void SetDepthAlgorithm(ComparisonFunc algorithm);
	void SetStencilMask(uint32_t mask);
	void SetStencilOperations(Operation stencilFail, Operation depthFail, Operation bothPass);

	void SetBlendingFunction(BlendFactor sourceFactor, BlendFactor destinationFactor);
	void SetBlendingEquation(BlendEquation equation);

	void SetCullFace(CullFace cullFace);

	void SetDepthWriting(bool enable);
	void SetColorWriting(bool enableRed, bool enableGreen, bool enableBlue, bool enableAlpha);

	void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

	void DrawElements(PrimitiveMode primitiveMode, uint32_t indexCount);
	void DrawElementsInstanced(PrimitiveMode primitiveMode, uint32_t indexCount, uint32_t instances);
	void DrawArrays(PrimitiveMode primitiveMode, uint32_t vertexCount);
	void DrawArraysInstanced(PrimitiveMode primitiveMode, uint32_t vertexCount, uint32_t instances);

	//void DispatchCompute(uint32_t x, uint32_t y, uint32_t z);
	//void MemoryBarrier(MemoryBarrierFlags barriers);

} // namespace OGLContext