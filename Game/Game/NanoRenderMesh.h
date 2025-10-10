#pragma once

#include "NanoRenderMaterial.h"
#include "NanoMath.h"

struct MeshVertex final
{
	glm::vec3 position{ 0.0f };
	glm::vec3 color{ 1.0f };
	glm::vec3 normal{ 0.0f };
	glm::vec2 texCoord{ 0.0f };
	glm::vec3 tangent{ 0.0f };
	glm::vec3 bitangent{ 0.0f };
};
void SetMeshVertexAttributes();

struct MeshCreateInfo final
{
	std::vector<MeshVertex> vertices;
	std::vector<uint32_t>   indices;
	std::optional<Material> material{};
};

class Mesh final
{
public:
	Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices, std::optional<Material> material);
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&& mesh) noexcept;
	~Mesh();

	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&& mesh) noexcept;

	void tDraw(GLenum mode = GL_TRIANGLES, GLuint program = 0, bool bindMaterial = true, bool instancing = false, int amount = 1);

	uint32_t GetVertexCount() const noexcept { return m_vertexCount; }
	uint32_t GetIndexCount() const noexcept { return m_indicesCount; }
	std::optional<Material> GetMaterial() noexcept { return m_material; }
	const AABB& GetAABB() const noexcept { return m_aabb; }

private:
	void initAABB(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices);
	uint32_t                m_vertexCount{ 0 };
	uint32_t                m_indicesCount{ 0 };
	GLuint                  m_vao{ 0 };
	GLuint                  m_vbo{ 0 };
	GLuint                  m_ebo{ 0 };
	std::optional<Material> m_material{};
	AABB                    m_aabb{};
};