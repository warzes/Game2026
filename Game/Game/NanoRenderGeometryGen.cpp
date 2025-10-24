#include "stdafx.h"
#include "NanoRenderGeometryGen.h"
#include "NanoRenderTextures.h"
//=============================================================================
MeshCreateInfo GeometryGenerator::CreatePlane(float width, float height, float wSegment, float hSegment)
{
	MeshCreateInfo meshInfo;

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
MeshCreateInfo GeometryGenerator::CreateQuad(const glm::vec2& size)
{
	glm::vec2 s2 = size / 2.0f;

	MeshCreateInfo meshInfo;

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
inline void buildBoxPlane(MeshCreateInfo& meshInfo, int& numberOfVertices, int u, int v, int w, float udir, float vdir, float width, float height, float depth, float gridX, float gridY)
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
MeshCreateInfo GeometryGenerator::CreateBox(float width, float height, float depth, float widthSeg, float heightSeg, float depthSeg)
{
	MeshCreateInfo meshInfo;

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
MeshCreateInfo GeometryGenerator::CreateSphere(float radius, float widthSeg, float heightSeg, float phiStart, float phiLength, float thetaStart, float thetaLength)
{
	MeshCreateInfo meshInfo;

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
void GeometryGenerator::ComputeNormals(MeshCreateInfo& meshInfo)
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
void GeometryGenerator::ComputeTangents(MeshCreateInfo& meshInfo)
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