#pragma once

#include "OGLEnum.h"

//=============================================================================
// VertexAttribute
//=============================================================================

struct VertexAttribute final
{
	DataType    type{ DataType::Float };
	GLint       count{ 1 };
	const void* offset{ nullptr }; //  EXAMPLE: (void*)offsetof(Vertex, uv)
	bool        normalized{ false };
	bool        perInstance{ false };
};
void SpecifyVertexAttributes(size_t vertexSize, std::span<const VertexAttribute> attributes);

//=============================================================================
// Vertex Formats
//=============================================================================

struct VertexP3 final
{
	glm::vec3 position;

	static void SetVertexAttributes();
};

struct VertexPNT final
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;

	static void SetVertexAttributes();
};

struct QuadVertex final
{
	glm::vec2 position{ 0.0f };
	glm::vec2 texCoord{ 0.0f };

	static void SetVertexAttributes();
};

struct MeshVertex final
{
	glm::vec3 position{ 0.0f };
	glm::vec3 color{ 1.0f };
	glm::vec3 normal{ 0.0f };
	glm::vec2 texCoord{ 0.0f };
	glm::vec3 tangent{ 0.0f };
	glm::vec3 bitangent{ 0.0f };

	static void SetVertexAttributes();
};