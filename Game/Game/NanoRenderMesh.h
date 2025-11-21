#pragma once

#include "NanoRenderMaterial.h"
#include "NanoMath.h"
#include "NanoOpenGL3.h"

struct MeshInfo final
{
	std::vector<MeshVertex>    vertices;
	std::vector<uint32_t>      indices;
	std::optional<Material>    material{};
	std::optional<PBRMaterial> pbrMaterial{};
};

class Mesh final
{
public:
	Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices, std::optional<Material> material, std::optional<PBRMaterial> pbrMaterial);
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&& other) noexcept;
	~Mesh();

	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&& other) noexcept;

	void Draw(GLenum mode = GL_TRIANGLES, unsigned instanceCount = 1) const;

	void tDraw(GLenum mode = GL_TRIANGLES, ProgramHandle program = {}, bool bindMaterial = true, bool instancing = false, int amount = 1);

	auto GetVertexCount() const noexcept { return m_vertexCount; }
	auto GetIndexCount() const noexcept { return m_indicesCount; }
	auto GetMaterial() const noexcept { return m_material; }
	auto GetPbrMaterial() const noexcept { return m_pbrMaterial; }
	const AABB& GetAABB() const noexcept { return m_aabb; }

private:
	void initAABB(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices);

	uint32_t                   m_vertexCount{ 0 };
	uint32_t                   m_indicesCount{ 0 };
	GLuint                     m_vao{ 0 };
	BufferHandle               m_vbo{};
	BufferHandle               m_ebo{};
	std::optional<Material>    m_material{};
	std::optional<PBRMaterial> m_pbrMaterial{};
	AABB                       m_aabb{};
};