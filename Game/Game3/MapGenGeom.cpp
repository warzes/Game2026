#include "stdafx.h"
#include "Map.h"
//=============================================================================
size_t addMeshInfo(std::vector<MeshInfo>& meshInfo, Texture2D texId)
{
	for (size_t i = 0; i < meshInfo.size(); i++)
	{
		if (meshInfo[i].material->diffuseTextures[0] == texId)
		{
			return i;
		}
	}

	MeshInfo nmi{};
	nmi.material = Material();
	nmi.material->diffuseTextures.push_back(texId);
	meshInfo.push_back(nmi);
	return meshInfo.size() - 1;
}
//=============================================================================
void AddBox(const glm::vec3& center, float width, float height, float depth, const glm::vec3& color, std::vector<MeshVertex>& verticesWall, std::vector<unsigned int>& indicesWall, std::vector<MeshVertex>& verticesCeil, std::vector<unsigned int>& indicesCeil, std::vector<MeshVertex>& verticesFloor, std::vector<unsigned int>& indicesFloor, bool enablePlane[6])
{
	float halfWidth  = width / 2.0f;
	float halfHeight = height / 2.0f;
	float halfDepth  = depth / 2.0f;

	// Define the 8 corner points relative to the center
	glm::vec3 p0 = center + glm::vec3(-halfWidth, -halfHeight, -halfDepth); // 0: Left-Bottom-Front
	glm::vec3 p1 = center + glm::vec3(halfWidth, -halfHeight, -halfDepth);  // 1: Right-Bottom-Front
	glm::vec3 p2 = center + glm::vec3(halfWidth, halfHeight, -halfDepth);   // 2: Right-Top-Front
	glm::vec3 p3 = center + glm::vec3(-halfWidth, halfHeight, -halfDepth);  // 3: Left-Top-Front

	glm::vec3 p4 = center + glm::vec3(-halfWidth, -halfHeight, halfDepth);  // 4: Left-Bottom-Back
	glm::vec3 p5 = center + glm::vec3(halfWidth, -halfHeight, halfDepth);   // 5: Right-Bottom-Back
	glm::vec3 p6 = center + glm::vec3(halfWidth, halfHeight, halfDepth);    // 6: Right-Top-Back
	glm::vec3 p7 = center + glm::vec3(-halfWidth, halfHeight, halfDepth);   // 7: Left-Top-Back

	// Face Colors (optional, for visual distinction)
#if 0 // DEBUG
	glm::vec3 red(1.0f, 0.0f, 0.0f);
	glm::vec3 green(0.0f, 1.0f, 0.0f);
	glm::vec3 blue(0.0f, 0.0f, 1.0f);
	glm::vec3 yellow(1.0f, 1.0f, 0.0f);
	glm::vec3 cyan(0.0f, 1.0f, 1.0f);
	glm::vec3 magenta(1.0f, 0.0f, 1.0f);
#else
	const glm::vec3& red = color;
	const glm::vec3& green = color;
	const glm::vec3& blue = color;
	const glm::vec3& yellow = color;
	const glm::vec3& cyan = color;
	const glm::vec3& magenta = color;
#endif

	// Front Face Z = center.z - halfDepth (p0, p1, p2, p3) - CCW order for outward facing normal
	GeometryGenerator::AddPlane(p0, p3, p2, p1, verticesWall, indicesWall, blue, glm::vec2(0, 0), glm::vec2(0, height), glm::vec2(width, height), glm::vec2(width, 0));
	// Back Face Z = center.z + halfDepth (p5, p4, p7, p6) - CCW order for outward facing normal
	GeometryGenerator::AddPlane(p5, p6, p7, p4, verticesWall, indicesWall, red, glm::vec2(width, 0), glm::vec2(width, height), glm::vec2(0, height), glm::vec2(0, 0));
	// Top Face Y = center.y + halfHeight (p3, p2, p6, p7) - CCW order for outward facing normal
	GeometryGenerator::AddPlane(p3, p7, p6, p2, verticesFloor, indicesFloor, green, glm::vec2(0, width), glm::vec2(0, 0), glm::vec2(depth, 0), glm::vec2(depth, width));
	// Bottom Face Y = center.y - halfHeight (p4, p5, p1, p0) - CCW order for outward facing normal
	GeometryGenerator::AddPlane(p4, p0, p1, p5, verticesCeil, indicesCeil, yellow, glm::vec2(0, depth), glm::vec2(0, 0), glm::vec2(width, 0), glm::vec2(width, depth));
	// Right Face X = center.x + halfWidth (p1, p5, p6, p2) - CCW order for outward facing normal
	GeometryGenerator::AddPlane(p1, p2, p6, p5, verticesWall, indicesWall, cyan, glm::vec2(0, 0), glm::vec2(0, height), glm::vec2(depth, height), glm::vec2(depth, 0));
	// Left Face X = center.x - halfWidth (p4, p0, p3, p7) - CCW order for outward facing normal
	GeometryGenerator::AddPlane(p4, p7, p3, p0, verticesWall, indicesWall, magenta, glm::vec2(0, 0), glm::vec2(0, height), glm::vec2(depth, height), glm::vec2(depth, 0));
}
//=============================================================================