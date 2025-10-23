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