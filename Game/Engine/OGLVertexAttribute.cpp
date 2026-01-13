#include "stdafx.h"
#include "OGLVertexAttribute.h"
//=============================================================================
void SpecifyVertexAttributes(size_t vertexSize, std::span<const VertexAttribute> attributes)
{
	assert(vertexSize > 0);
	assert(attributes.size() > 0);

	for (size_t i = 0; i < attributes.size(); i++)
	{
		const auto& attr = attributes[i];
		const GLuint index = static_cast<GLuint>(i);

		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, attr.count, EnumToValue(attr.type), attr.normalized ? GL_TRUE : GL_FALSE, static_cast<GLsizei>(vertexSize), attr.offset);
		glVertexAttribDivisor(index, attr.perInstance ? 1 : 0);
	}
}
//=============================================================================
void VertexP3::SetVertexAttributes()
{
	const size_t vertexSize = sizeof(VertexP3);
	const VertexAttribute attributes[] =
	{
		{.type = DataType::Float, .count = 3, .offset = (void*)offsetof(VertexP3, position)},
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================
void VertexPNT::SetVertexAttributes()
{
	const size_t vertexSize = sizeof(VertexPNT);
	const VertexAttribute attributes[] =
	{
		{.type = DataType::Float, .count = 3, .offset = (void*)offsetof(VertexPNT, position)},
		{.type = DataType::Float, .count = 3, .offset = (void*)offsetof(VertexPNT, normal)},
		{.type = DataType::Float, .count = 2, .offset = (void*)offsetof(VertexPNT, texcoord)},
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================
void QuadVertex::SetVertexAttributes()
{
	const size_t vertexSize = sizeof(QuadVertex);
	const VertexAttribute attributes[] =
	{
		{.type = DataType::Float, .count = 2, .offset = (void*)offsetof(QuadVertex, position)},
		{.type = DataType::Float, .count = 2, .offset = (void*)offsetof(QuadVertex, texCoord)},
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================
void MeshVertex::SetVertexAttributes()
{
	const size_t vertexSize = sizeof(MeshVertex);
	const VertexAttribute attributes[] =
	{
		{.type = DataType::Float, .count = 3, .offset = (void*)offsetof(MeshVertex, position)},
		{.type = DataType::Float, .count = 3, .offset = (void*)offsetof(MeshVertex, color)},
		{.type = DataType::Float, .count = 3, .offset = (void*)offsetof(MeshVertex, normal)},
		{.type = DataType::Float, .count = 2, .offset = (void*)offsetof(MeshVertex, texCoord)},
		{.type = DataType::Float, .count = 3, .offset = (void*)offsetof(MeshVertex, tangent)},
		{.type = DataType::Float, .count = 3, .offset = (void*)offsetof(MeshVertex, bitangent)},
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================