#include "stdafx.h"
#include "NanoRenderMesh.h"
//=============================================================================
Mesh::Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices, std::optional<Material> material, std::optional<PBRMaterial> pbrMaterial)
{
	assert(!vertices.empty());

	m_vertexCount = static_cast<uint32_t>(vertices.size());
	m_indicesCount = static_cast<uint32_t>(indices.size());

	m_material = material;
	m_pbrMaterial = pbrMaterial;

	GLuint currentVBO = GetCurrentBuffer(GL_ARRAY_BUFFER);
	GLuint currentEBO = GetCurrentBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// Buffers
	m_vbo = CreateBuffer(BufferTarget::Array, BufferUsage::StaticDraw, vertices.size() * sizeof(MeshVertex), vertices.data());
	if (!indices.empty())
		m_ebo = CreateBuffer(BufferTarget::ElementArray, BufferUsage::StaticDraw, indices.size() * sizeof(uint32_t), indices.data());

	// VAO
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo.handle);
	if (m_ebo.handle > 0) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo.handle);
	MeshVertex::SetVertexAttributes();
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
	, m_vbo(std::exchange(old.m_vbo.handle, 0))
	, m_ebo(std::exchange(old.m_ebo.handle, 0))
	, m_material(std::exchange(old.m_material, std::nullopt))
	, m_pbrMaterial(std::exchange(old.m_pbrMaterial, std::nullopt))
	, m_aabb(old.m_aabb)
{
}
//=============================================================================
Mesh::~Mesh()
{
	if (m_vbo.handle) glDeleteBuffers(1, &m_vbo.handle);
	if (m_ebo.handle) glDeleteBuffers(1, &m_ebo.handle);
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
		m_vbo.handle = std::exchange(old.m_vbo.handle, 0);
		m_ebo.handle = std::exchange(old.m_ebo.handle, 0);
		m_material = std::exchange(old.m_material, std::nullopt);
		m_pbrMaterial = std::exchange(old.m_pbrMaterial, std::nullopt);
		m_aabb = old.m_aabb;
	}
	return *this;
}
//=============================================================================
void Mesh::Draw(GLenum mode, unsigned instanceCount) const
{
	assert(m_vao);
	glBindVertexArray(m_vao);

	if (m_ebo.handle > 0)
	{
		if (instanceCount > 1)
			glDrawElementsInstanced(mode, static_cast<GLsizei>(m_indicesCount), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(instanceCount));
		else
			glDrawElements(mode, static_cast<GLsizei>(m_indicesCount), GL_UNSIGNED_INT, 0);
	}
	else
	{
		if (instanceCount > 1)
			; // TODO:???
		else
			glDrawArrays(mode, 0, static_cast<GLsizei>(m_vertexCount));
	}
	glBindVertexArray(0);
}
//=============================================================================
void Mesh::tDraw(GLenum mode, ProgramHandle program, bool bindMaterial, bool instancing, int amount)
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

		if (program.handle)
		{
			SetUniform(GetUniformLocation(program, "material.color_diffuse"), m_material->diffuseColor);
			SetUniform(GetUniformLocation(program, "material.color_specular"), m_material->specularColor);
			SetUniform(GetUniformLocation(program, "material.color_ambient"), m_material->ambientColor);
			SetUniform(GetUniformLocation(program, "material.shininess"), m_material->shininess);

			SetUniform(GetUniformLocation(program, "material.hasDiffuse"), hasDiffuseTexture ? 1 : 0);
			SetUniform(GetUniformLocation(program, "material.hasSpecular"), hasSpecularTexture ? 1 : 0);
			SetUniform(GetUniformLocation(program, "material.hasNormal"), hasNormalTexture ? 1 : 0);
			SetUniform(GetUniformLocation(program, "material.nbTextures"), nbTextures);
			SetUniform(GetUniformLocation(program, "material.opacity"), m_material->opacity);
		}
	}

	glBindVertexArray(m_vao);
	if (m_ebo.handle > 0)
	{
		if (instancing)
			glDrawElementsInstanced(mode, static_cast<GLsizei>(m_indicesCount), GL_UNSIGNED_INT, 0, amount);
		else
			glDrawElements(mode, static_cast<GLsizei>(m_indicesCount), GL_UNSIGNED_INT, 0);
	}
	else
	{
		if (instancing)
			; // TODO:???
		else
			glDrawArrays(mode, 0, static_cast<GLsizei>(m_vertexCount));
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