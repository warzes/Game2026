#include "stdafx.h"
#include "NanoRenderMesh.h"
//=============================================================================
void SetMeshVertexAttributes()
{
	const size_t vertexSize = sizeof(MeshVertex);
	const VertexAttribute attributes[] =
	{
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, position)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, color)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, normal)},
		{.type = GL_FLOAT, .size = 2, .offset = (void*)offsetof(MeshVertex, texCoord)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, tangent)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, bitangent)},
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================
Mesh::Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices, std::optional<Material> material)
{
	assert(!vertices.empty());

	m_vertexCount = vertices.size();
	m_indicesCount = indices.size();

	m_material = material;

	GLuint currentVBO = GetCurrentBuffer(GL_ARRAY_BUFFER);
	GLuint currentEBO = GetCurrentBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// Buffers
	m_vbo = CreateBuffer(GL_ARRAY_BUFFER, BufferUsage::Static, vertices.size() * sizeof(MeshVertex), vertices.data());
	if (!indices.empty())
		m_ebo = CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferUsage::Static, indices.size() * sizeof(uint32_t), indices.data());

	// VAO
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	if (m_ebo > 0) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	SetMeshVertexAttributes();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, currentVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentEBO);

	initAABB(vertices, indices);
}
//=============================================================================
Mesh::Mesh(Mesh&& old) noexcept
	: m_vertexCount(std::exchange(old.m_vertexCount, 0))
	, m_indicesCount(std::exchange(old.m_indicesCount, 0))
	, m_vao(std::exchange(old.m_vao, 0))
	, m_vbo(std::exchange(old.m_vbo, 0))
	, m_ebo(std::exchange(old.m_ebo, 0))
	, m_material(std::exchange(old.m_material, std::nullopt))
	, m_aabb(old.m_aabb)
{
}
//=============================================================================
Mesh::~Mesh()
{
	if (m_vbo) glDeleteBuffers(1, &m_vbo);
	if (m_ebo) glDeleteBuffers(1, &m_ebo);
	if (m_vao) glDeleteVertexArrays(1, &m_vao);
}
//=============================================================================
Mesh& Mesh::operator=(Mesh&& old) noexcept
{
	if (this != &old)
	{
		this->~Mesh();
		m_vertexCount = std::exchange(old.m_vertexCount, 0);
		m_indicesCount = std::exchange(old.m_indicesCount, 0);
		m_vao = std::exchange(old.m_vao, 0);
		m_vbo = std::exchange(old.m_vbo, 0);
		m_ebo = std::exchange(old.m_ebo, 0);
		m_material = std::exchange(old.m_material, std::nullopt);
		m_aabb = old.m_aabb;
	}
	return *this;
}
//=============================================================================
void Mesh::tDraw(GLenum mode, GLuint program, bool bindMaterial, bool instancing, int amount)
{
	assert(m_vao);

	// TODO: переделать. убрать биндинг материала в отдельную функцию

	if (bindMaterial && m_material)
	{
		bool hasDiffuseTexture = false;
		bool hasSpecularTexture = false;
		bool hasNormalTexture = false;
		//bool hasAlbedoTexture = false;
		//bool hasMetallicRoughTexture = false;
		int nbTextures = 0;

		if (!m_material->diffuseTextures.empty())
		{
			hasDiffuseTexture = true;
			BindTexture2D(0, m_material->diffuseTextures[0].id);
			nbTextures++;
		}
		if (!m_material->specularTextures.empty())
		{
			hasSpecularTexture = true;
			BindTexture2D(1, m_material->specularTextures[0].id);
			nbTextures++;
		}
		if (!m_material->normalTextures.empty())
		{
			hasNormalTexture = true;
			BindTexture2D(2, m_material->normalTextures[0].id);
			nbTextures++;
		}

		if (program)
		{
			// if BLINN_PHONG
			SetUniform(GetUniformLocation(program, "material.color_diffuse"), m_material->diffuseColor);
			SetUniform(GetUniformLocation(program, "material.color_specular"), m_material->specularColor);
			SetUniform(GetUniformLocation(program, "material.color_ambient"), m_material->ambientColor);
			SetUniform(GetUniformLocation(program, "material.shininess"), m_material->shininess);
			// elif PBR
/*			SetUniform(GetUniformLocation(program, "material.albedo"), m_material->diffuseColor);
			SetUniform(GetUniformLocation(program, "material.metallic"), m_material->metallic);
			SetUniform(GetUniformLocation(program, "material.roughness"), m_material->roughness);
			SetUniform(GetUniformLocation(program, "material.ao"), 1.0f);*/

			SetUniform(GetUniformLocation(program, "material.hasDiffuse"), hasDiffuseTexture ? 1 : 0);
			SetUniform(GetUniformLocation(program, "material.hasSpecular"), hasSpecularTexture ? 1 : 0);
			SetUniform(GetUniformLocation(program, "material.hasNormal"), hasNormalTexture ? 1 : 0);
			SetUniform(GetUniformLocation(program, "material.nbTextures"), nbTextures);
			SetUniform(GetUniformLocation(program, "material.opacity"), m_material->opacity);
		}
	}

	glBindVertexArray(m_vao);
	if (m_ebo > 0)
	{
		if (instancing)
			glDrawElementsInstanced(mode, m_indicesCount, GL_UNSIGNED_INT, 0, amount);
		else
			glDrawElements(mode, m_indicesCount, GL_UNSIGNED_INT, 0);
	}
	else
	{
		if (instancing)
			; // TODO:???
		else
			glDrawArrays(mode, 0, m_vertexCount);
	}
	glBindVertexArray(0);
}
//=============================================================================
void Mesh::initAABB(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indexData)
{
	if (indexData.size() > 0)
	{
		for (size_t index_id = 0; index_id < indexData.size(); index_id++)
		{
			m_aabb.CombinePoint(vertices[indexData[index_id]].position);
		}
	}
	else
	{
		for (size_t vertex_id = 0; vertex_id < vertices.size(); vertex_id++)
		{
			m_aabb.CombinePoint(vertices[vertex_id].position);
		}
	}
}
//=============================================================================