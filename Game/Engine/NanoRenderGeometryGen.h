#pragma once

#include "NanoRenderMesh.h"

namespace GeometryGenerator
{
	MeshInfo CreatePlane(float width = 1.0f, float height = 1.0f, float wSegment = 1.0f, float hSegment = 1.0f);
	MeshInfo CreateQuad(const glm::vec2& size);
	MeshInfo CreateBox(float width = 1.0f, float height = 1.0f, float depth = 1.0, float widthSegments = 1.0f, float heightSegments = 1.0f, float depthSegments = 1.0f);
	MeshInfo CreateSphere(float radius = 1.0f, float widthSegments = 8.0f, float heightSegments = 6.0f, float phiStart = 0.0f, float phiLength = M_PI * 2.0f, float thetaStart = 0.0f, float thetaLength = M_PI);

	void ComputeNormals(MeshInfo& meshInfo);
	void ComputeTangents(MeshInfo& meshInfo);

	void AddPlane(std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices,
		const glm::vec3& center = glm::vec3(0.0f), float width = 1.0f, float height = 1.0f,
		const glm::vec3& planeNormal = glm::vec3(0.0f, 1.0f, 0.0f));

	void AddPlane(const glm::vec3& p0, const glm::vec3& p1,
		const glm::vec3& p2, const glm::vec3& p3,
		std::vector<MeshVertex>& vertices,
		std::vector<unsigned int>& indices,
		const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f),
		const glm::vec2& uv0 = glm::vec2(0.0f, 0.0f),
		const glm::vec2& uv1 = glm::vec2(1.0f, 0.0f),
		const glm::vec2& uv2 = glm::vec2(1.0f, 1.0f),
		const glm::vec2& uv3 = glm::vec2(0.0f, 1.0f));

} // namespace GeometryGenerator