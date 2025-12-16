#include "stdafx.h"
#include "NanoRenderGeometryGen.h"
#include "NanoRenderTextures.h"
//=============================================================================
MeshInfo GeometryGenerator::CreatePlane(float width, float height, float wSegment, float hSegment)
{
	MeshInfo meshInfo;

	float width_half = width / 2.0f;
	float height_half = height / 2.0f;

	float gridX1 = wSegment + 1.0f;
	float gridY1 = hSegment + 1.0f;

	float segment_width = width / wSegment;
	float segment_height = height / hSegment;

	MeshVertex vertex;

	// generate Position Normal TexCoords
	for (int iy = 0; iy < static_cast<int>(gridY1); iy++)
	{
		float y = static_cast<float>(iy) * segment_height - height_half;

		for (int ix = 0; ix < static_cast<int>(gridX1); ix++)
		{
			float x = static_cast<float>(ix) * segment_width - width_half;
			vertex.position = glm::vec3(x, 0.0f, -y);
			vertex.color = glm::vec3(1.0f);
			vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			vertex.texCoord = glm::vec2(static_cast<float>(ix) / wSegment, 1.0f - (static_cast<float>(iy) / hSegment));
			vertex.tangent = glm::vec3(0.0f); // TODO:
			meshInfo.vertices.push_back(vertex);
		}
	}
	// generate indices
	for (int iy = 0; iy < static_cast<int>(hSegment); iy++)
	{
		for (int ix = 0; ix < static_cast<int>(wSegment); ix++)
		{
			float a = static_cast<float>(ix) + gridX1 * static_cast<float>(iy);
			float b = static_cast<float>(ix) + gridX1 * static_cast<float>(iy + 1);
			float c = static_cast<float>(ix + 1) + gridX1 * static_cast<float>(iy + 1);
			float d = static_cast<float>(ix + 1) + gridX1 * static_cast<float>(iy);
			meshInfo.indices.push_back(static_cast<uint32_t>(a));
			meshInfo.indices.push_back(static_cast<uint32_t>(b));
			meshInfo.indices.push_back(static_cast<uint32_t>(d));
			meshInfo.indices.push_back(static_cast<uint32_t>(b));
			meshInfo.indices.push_back(static_cast<uint32_t>(c));
			meshInfo.indices.push_back(static_cast<uint32_t>(d));
		}
	}

	Material mat;
	mat.diffuseTextures.push_back(textures::GetDefaultDiffuse2D());
	meshInfo.material = mat;

	return meshInfo;
}
//=============================================================================
MeshInfo GeometryGenerator::CreateQuad(const glm::vec2& size)
{
	glm::vec2 s2 = size / 2.0f;

	MeshInfo meshInfo;

	meshInfo.vertices.resize(4);
	meshInfo.vertices[0].position = { -s2.x, -s2.y, 0.0f };
	meshInfo.vertices[0].texCoord = { 0.0f, 0.0f };
	meshInfo.vertices[1].position = { s2.x, -s2.y, 0.0f };
	meshInfo.vertices[1].texCoord = { 1.0f, 0.0f };
	meshInfo.vertices[2].position = { s2.x, s2.y, 0.0f };
	meshInfo.vertices[2].texCoord = { 1.0f, 1.0f };
	meshInfo.vertices[3].position = { -s2.x, s2.y, 0.0f };
	meshInfo.vertices[3].texCoord = { 0.0f, 1.0f };

	meshInfo.indices = {
		0, 1, 2,
		0, 2, 3
	};

	GeometryGenerator::ComputeNormals(meshInfo);
	GeometryGenerator::ComputeTangents(meshInfo);

	Material mat;
	mat.diffuseTextures.push_back(textures::GetDefaultDiffuse2D());
	meshInfo.material = mat;

	return meshInfo;
}
//=============================================================================
inline void buildBoxPlane(MeshInfo& meshInfo, int& numberOfVertices, int u, int v, int w, float udir, float vdir, float width, float height, float depth, float gridX, float gridY)
{
	float segmentWidth = width / gridX;
	float segmentHeight = height / gridY;

	float widthHalf = width / 2.0f;
	float heightHalf = height / 2.0f;
	float depthHalf = depth / 2.0f;

	float gridX1 = gridX + 1.0f;
	float gridY1 = gridY + 1.0f;

	MeshVertex vertex;

	for (int iy = 0; iy < static_cast<int>(gridY1); iy++)
	{
		float y = static_cast<float>(iy) * segmentHeight - heightHalf;
		for (int ix = 0; ix < static_cast<int>(gridX1); ix++)
		{
			float x = static_cast<float>(ix) * segmentWidth - widthHalf;

			// position
			vertex.position[u] = x * udir;
			vertex.position[v] = y * vdir;
			vertex.position[w] = depthHalf;

			// normals
			vertex.normal[u] = 0.0f;
			vertex.normal[v] = 0.0f;
			vertex.normal[w] = depth > 0 ? 1.0f : -1.0f;

			// uvs
			vertex.texCoord = glm::vec2(static_cast<float>(ix) / gridX, 1.0f - static_cast<float>(iy) / gridY);

			meshInfo.vertices.push_back(vertex);
		}
	}

	// indices
	for (int iy = 0; iy < static_cast<int>(gridY); iy++)
	{
		for (int ix = 0; ix < static_cast<int>(gridX); ix++)
		{
			float a = static_cast<float>(numberOfVertices) + static_cast<float>(ix) + static_cast<float>(iy) * gridX1;
			float b = static_cast<float>(numberOfVertices) + static_cast<float>(ix) + static_cast<float>(iy + 1) * gridX1;
			float c = static_cast<float>(numberOfVertices) + static_cast<float>(ix + 1) + static_cast<float>(iy + 1) * gridX1;
			float d = static_cast<float>(numberOfVertices) + static_cast<float>(ix + 1) + static_cast<float>(iy) * gridX1;

			meshInfo.indices.push_back(static_cast<unsigned>(a));
			meshInfo.indices.push_back(static_cast<unsigned>(d));
			meshInfo.indices.push_back(static_cast<unsigned>(b));

			meshInfo.indices.push_back(static_cast<unsigned>(b));
			meshInfo.indices.push_back(static_cast<unsigned>(d));
			meshInfo.indices.push_back(static_cast<unsigned>(c));
		}
	}
	numberOfVertices += static_cast<int>(gridX1 * gridY1);
}
//=============================================================================
MeshInfo GeometryGenerator::CreateBox(float width, float height, float depth, float widthSeg, float heightSeg, float depthSeg)
{
	MeshInfo meshInfo;

	float widthSegments = static_cast<float>(std::max(1, static_cast<int>(std::floor(widthSeg))));
	float heightSegments = static_cast<float>(std::max(1, static_cast<int>(std::floor(heightSeg))));
	float depthSegments = static_cast<float>(std::max(1, static_cast<int>(std::floor(depthSeg))));

	int numberOfVertices = 0;

	// Плоскости: +X, -X, +Y, -Y, +Z, -Z
	buildBoxPlane(meshInfo, numberOfVertices, 2, 1, 0, -1, -1, depth, height, width, depthSegments, heightSegments); // +X
	buildBoxPlane(meshInfo, numberOfVertices, 2, 1, 0, 1, -1, depth, height, -width, depthSegments, heightSegments); // -X

	buildBoxPlane(meshInfo, numberOfVertices, 0, 2, 1, 1, 1, width, depth, height, widthSegments, depthSegments); // +Y
	buildBoxPlane(meshInfo, numberOfVertices, 0, 2, 1, 1, -1, width, depth, -height, widthSegments, depthSegments); // -Y

	buildBoxPlane(meshInfo, numberOfVertices, 0, 1, 2, 1, -1, width, height, depth, widthSegments, heightSegments); // +Z
	buildBoxPlane(meshInfo, numberOfVertices, 0, 1, 2, -1, -1, width, height, -depth, widthSegments, heightSegments); // -Z

	Material mat;
	mat.diffuseTextures.push_back(textures::GetDefaultDiffuse2D());
	meshInfo.material = mat;

	return meshInfo;
}
//=============================================================================
MeshInfo GeometryGenerator::CreateSphere(float radius, float widthSeg, float heightSeg, float phiStart, float phiLength, float thetaStart, float thetaLength)
{
	MeshInfo meshInfo;

	constexpr const float PI = glm::pi<float>();
	const float thetaEnd = glm::min(thetaStart + thetaLength, PI);

	float widthSegments = static_cast<float>(std::max(3u, static_cast<unsigned>(std::floor(widthSeg))));
	float heightSegments = static_cast<float>(std::max(2u, static_cast<unsigned>(std::floor(heightSeg))));

	std::vector<std::vector<unsigned>> grid;
	int index = 0;

	MeshVertex vertex;
	for (unsigned iy = 0; iy <= static_cast<unsigned>(heightSegments); iy++)
	{
		std::vector<unsigned> verticesRow;

		float v = static_cast<float>(iy) / heightSegments;

		float uOffset = 0.0f;

		// Коррекция UV на полюсах (для избежания сжатия)
		if (iy == 0 && thetaStart == 0.0f)
			uOffset = 0.5f / widthSegments;
		else if (iy == static_cast<unsigned>(heightSegments) && thetaEnd == PI)
			uOffset = -0.5f / widthSegments;

		for (unsigned ix = 0; ix <= static_cast<unsigned>(widthSegments); ix++)
		{
			const float u = static_cast<float>(ix) / widthSegments;

			const float phi = phiStart + u * phiLength;
			const float theta = thetaStart + v * thetaLength;

			// Позиция (Y — вверх, правосторонняя система) // TODO: переделать под левосторонюю?
			vertex.position.x = -radius * glm::cos(phi) * glm::sin(theta);
			vertex.position.y = radius * glm::cos(theta);
			vertex.position.z = radius * glm::sin(phi) * glm::sin(theta);

			// normal
			vertex.normal = glm::normalize(vertex.position);

			// uv
			vertex.texCoord = glm::vec2(u + uOffset, 1.0f - v);

			meshInfo.vertices.push_back(vertex);
			verticesRow.push_back(static_cast<unsigned>(index++));
		}
		grid.push_back(verticesRow);
	}

	// indices
	for (unsigned iy = 0; iy < static_cast<unsigned>(heightSegments); iy++)
	{
		for (unsigned ix = 0; ix < static_cast<unsigned>(widthSegments); ix++)
		{
			unsigned int a = grid[iy][ix + 1];
			unsigned int b = grid[iy][ix];
			unsigned int c = grid[iy + 1][ix];
			unsigned int d = grid[iy + 1][ix + 1];

			if (iy != 0 || thetaStart > 0)
			{
				meshInfo.indices.push_back(a);
				meshInfo.indices.push_back(d);
				meshInfo.indices.push_back(b);
			}

			if (iy != static_cast<unsigned>(heightSegments - 1) || thetaEnd < M_PI)
			{
				meshInfo.indices.push_back(b);
				meshInfo.indices.push_back(d);
				meshInfo.indices.push_back(c);
			}
		}
	}

	Material mat;
	mat.diffuseTextures.push_back(textures::GetDefaultDiffuse2D());
	meshInfo.material = mat;

	return meshInfo;
}
//=============================================================================
void GeometryGenerator::ComputeNormals(MeshInfo& meshInfo)
{
	for (auto& vertex : meshInfo.vertices)
	{
		vertex.normal = glm::vec3(0.0f);
	}

	for (unsigned int i = 0; i < meshInfo.indices.size(); i += 3)
	{
		uint32_t i0 = meshInfo.indices[i];
		uint32_t i1 = meshInfo.indices[i + 1];
		uint32_t i2 = meshInfo.indices[i + 2];

		auto& a = meshInfo.vertices[i0];
		auto& b = meshInfo.vertices[i1];
		auto& c = meshInfo.vertices[i2];

		glm::vec3 n = glm::normalize(glm::cross(b.position - c.position, b.position - a.position));

		a.normal += n;
		b.normal += n;
		c.normal += n;
	}

	for (auto& vertex : meshInfo.vertices)
	{
		vertex.normal = glm::normalize(vertex.normal);
	}
}
//=============================================================================
void GeometryGenerator::ComputeTangents(MeshInfo& meshInfo)
{
	for (auto& vertex : meshInfo.vertices)
	{
		vertex.tangent = glm::vec3(0.0f);
	}

	for (unsigned int i = 0; i < meshInfo.indices.size(); i += 3)
	{
		uint32_t i0 = meshInfo.indices[i];
		uint32_t i1 = meshInfo.indices[i + 1];
		uint32_t i2 = meshInfo.indices[i + 2];

		auto& a = meshInfo.vertices[i0];
		auto& b = meshInfo.vertices[i1];
		auto& c = meshInfo.vertices[i2];

		glm::vec3 edge1 = b.position - a.position;
		glm::vec3 edge2 = c.position - a.position;
		glm::vec2 deltaUV1 = b.texCoord - a.texCoord;
		glm::vec2 deltaUV2 = c.texCoord - a.texCoord;

		GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 tangent(
			f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
			f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
			f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
		);
		a.tangent = b.tangent = c.tangent = glm::normalize(tangent);

		glm::vec3 bitangent(
			f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
			f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
			f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z)
		);
		a.bitangent = b.bitangent = c.bitangent = glm::normalize(bitangent);
	}
}
//=============================================================================
void GeometryGenerator::AddPlane(std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices, const glm::vec3& center, float width, float height, const glm::vec3& planeNormal)
{
	glm::vec3 right;
	// Calculate a vector perpendicular to the normal (right)
	if (std::abs(planeNormal.y) < 0.9f) {
		right = glm::normalize(glm::cross(planeNormal, glm::vec3(0.0f, 1.0f, 0.0f)));
	}
	else {
		// Avoid degeneracy if normal is close to (0,1,0)
		right = glm::normalize(glm::cross(planeNormal, glm::vec3(1.0f, 0.0f, 0.0f)));
	}

	// Calculate the up vector for the plane (perpendicular to both normal and right)
	glm::vec3 up = glm::normalize(glm::cross(right, planeNormal));

	// Half sizes
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;

	// Calculate the four corners of the plane relative to its center
	glm::vec3 topLeft = center - right * halfWidth + up * halfHeight;      // v1
	glm::vec3 topRight = center + right * halfWidth + up * halfHeight;     // v0
	glm::vec3 bottomLeft = center - right * halfWidth - up * halfHeight;   // v2
	glm::vec3 bottomRight = center + right * halfWidth - up * halfHeight;  // v3

	// Current vertex count before adding new ones
	uint32_t indexOffset = static_cast<uint32_t>(vertices.size());

	// Define vertices with their properties
	// The normal is the provided planeNormal
	// Tangent and bitangent need to be set according to the local coordinate system of the face.

	MeshVertex v0;
	v0.position = topRight;
	v0.normal = planeNormal;
	// Tangent points along the 'right' direction of the plane
	v0.tangent = right;
	// Bitangent should align with the 'up' direction of the plane texture and follow the right-hand rule
	v0.bitangent = glm::cross(v0.normal, v0.tangent);
	v0.texCoord = glm::vec2(1.0f, 1.0f); // Top-right UV

	MeshVertex v1;
	v1.position = topLeft;
	v1.normal = planeNormal;
	v1.tangent = right;
	v1.bitangent = glm::cross(v1.normal, v1.tangent);
	v1.texCoord = glm::vec2(0.0f, 1.0f); // Top-left UV

	MeshVertex v2;
	v2.position = bottomLeft;
	v2.normal = planeNormal;
	v2.tangent = right;
	v2.bitangent = glm::cross(v2.normal, v2.tangent);
	v2.texCoord = glm::vec2(0.0f, 0.0f); // Bottom-left UV

	MeshVertex v3;
	v3.position = bottomRight;
	v3.normal = planeNormal;
	v3.tangent = right;
	v3.bitangent = glm::cross(v3.normal, v3.tangent);
	v3.texCoord = glm::vec2(1.0f, 0.0f); // Bottom-right UV

	// Add the 4 vertices to the array
	vertices.push_back(v0); // Index: indexOffset + 0
	vertices.push_back(v1); // Index: indexOffset + 1
	vertices.push_back(v2); // Index: indexOffset + 2
	vertices.push_back(v3); // Index: indexOffset + 3

	// Define the two triangles using the new indices
	// To make the side the normal points to the "front", reverse the winding order.
	// Triangle 1: v0 (top-right), v2 (bottom-left), v1 (top-left) - Reversed
	indices.push_back(indexOffset + 0);
	indices.push_back(indexOffset + 2);
	indices.push_back(indexOffset + 1);

	// Triangle 2: v0 (top-right), v3 (bottom-right), v2 (bottom-left) - Reversed
	indices.push_back(indexOffset + 0);
	indices.push_back(indexOffset + 3);
	indices.push_back(indexOffset + 2);
}
//=============================================================================
void GeometryGenerator::AddPlane(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, std::vector<MeshVertex>& vertices, std::vector<unsigned int>& indices, const glm::vec3& color, const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3)
{
	// Calculate the normal for the plane using 3 of the 4 points (assuming they are coplanar)
	// Using points p0, p1, p2 to define the plane's orientation
	glm::vec3 normal = CalculateFaceNormal(p0, p1, p2);

	// Calculate tangent and bitangent based on the first triangle (p0, p1, p2)
	glm::vec3 tangent, bitangent;
	CalculateTangentBitangent(p0, p1, p2, uv0, uv1, uv2, tangent, bitangent);

	unsigned int baseIndex = static_cast<unsigned int>(vertices.size());

	// Create 4 unique vertices for the quad (using triangle strip or fan implicitly by duplicating)
	// Triangle 1: p0, p1, p2
	// Triangle 2: p0, p2, p3
	// This requires 6 vertices total to handle potentially different UVs/normals/tangents per face corner if needed.
	// For a simple planar quad where all corners share the same calculated normal/tangent, we can do 4 unique verts.

	// However, to make two separate triangles that don't share edges incorrectly in a flat list,
	// we duplicate vertices as needed by index buffer, but since UVs might differ per corner even for a flat quad,
	// the safest is often to provide 6 vertices explicitly, one for each corner *for each triangle it belongs to*.
	// But let's assume a simpler case where the quad lies on a plane and tangents/UVs are consistent enough
	// or you want to average them per geometric vertex. The most common and expected behavior for "AddPlane"
	// is to create 4 unique vertices (one for each corner) and use an index buffer.

	MeshVertex v0;
	v0.position = p0;
	v0.color = color;
	v0.normal = normal;
	v0.texCoord = uv0;
	v0.tangent = tangent;
	v0.bitangent = bitangent;

	MeshVertex v1;
	v1.position = p1;
	v1.color = color;
	v1.normal = normal;
	v1.texCoord = uv1;
	v1.tangent = tangent;
	v1.bitangent = bitangent;

	MeshVertex v2;
	v2.position = p2;
	v2.color = color;
	v2.normal = normal;
	v2.texCoord = uv2;
	v2.tangent = tangent;
	v2.bitangent = bitangent;

	MeshVertex v3;
	v3.position = p3;
	v3.color = color;
	v3.normal = normal;
	v3.texCoord = uv3;
	v3.tangent = tangent;
	v3.bitangent = bitangent;

	// Add the 4 unique vertices
	vertices.push_back(v0);
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);

	// Define indices for two triangles forming the quad: CCW winding
	// Triangle 1: v0, v1, v2
	// Triangle 2: v0, v2, v3
	indices.push_back(baseIndex + 0); // v0
	indices.push_back(baseIndex + 1); // v1
	indices.push_back(baseIndex + 2); // v2
	indices.push_back(baseIndex + 0); // v0
	indices.push_back(baseIndex + 2); // v2
	indices.push_back(baseIndex + 3); // v3
}
//=============================================================================