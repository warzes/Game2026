#pragma once

#include "NanoRenderMesh.h"

namespace GeometryGenerator
{
	MeshCreateInfo CreatePlane(float width = 1.0f, float height = 1.0f, float wSegment = 1.0f, float hSegment = 1.0f);
	MeshCreateInfo CreateQuad(const glm::vec2& size);
	MeshCreateInfo CreateBox(float width = 1.0f, float height = 1.0f, float depth = 1.0, float widthSegments = 1.0f, float heightSegments = 1.0f, float depthSegments = 1.0f);
	MeshCreateInfo CreateSphere(float radius = 1.0f, float widthSegments = 8.0f, float heightSegments = 6.0f, float phiStart = 0.0f, float phiLength = M_PI * 2.0f, float thetaStart = 0.0f, float thetaLength = M_PI);

	void ComputeNormals(MeshCreateInfo& meshInfo);
	void ComputeTangents(MeshCreateInfo& meshInfo);

} // namespace GeometryGenerator