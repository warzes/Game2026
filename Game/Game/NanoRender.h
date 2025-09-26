#pragma once

#include "NanoOpenGL3.h"
#include "NanoMath.h"

struct Texture2D final
{
	GLuint   id{ 0 };
	uint32_t width{ 0 };
	uint32_t height{ 0 };
};

namespace textures
{
	bool Init();
	void Close();

	Texture2D GetDefaultDiffuse2D();
	Texture2D GetDefaultNormal2D();
	Texture2D GetDefaultSpecular2D();
	Texture2D LoadTexture2D(const std::string& fileName, ColorSpace colorSpace = ColorSpace::Linear, bool flipVertical = false);
} // namespace textures

struct Material final
{
	std::optional<Texture2D> diffuseTexture{};
	std::optional<Texture2D> specularTexture{};
	std::optional<Texture2D> normalTexture{};
	std::optional<Texture2D> depthTexture{};
	std::optional<Texture2D> emissionTexture{};

	float heightScale{ 0.1f };
	float emissionStrength{ 1.0f };

	glm::vec3 ambientColor{ 1.0f };
	glm::vec3 diffuseColor{ 1.0f };
	glm::vec3 specularColor{ 1.0f };
	float     shininess{ 64.f };
	float     refracti{ 0.0f };

	bool noLighing{ false };
};

struct MeshVertex final
{
	glm::vec3 position{ 0.0f };
	glm::vec3 color{ 1.0f };
	glm::vec3 normal{ 0.0f };
	glm::vec2 texCoord{ 0.0f };
	glm::vec3 tangent{ 0.0f };
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
	~Mesh();

	void Draw(GLenum mode = GL_TRIANGLES);

	uint32_t GetVertexCount() const { return m_vertexCount; }
	uint32_t GetIndexCount() const { return m_indicesCount; }
	std::optional<Material> GetMaterial() { return m_material; }
	const AABB& GetAABB() const { return m_aabb; }

private:
	uint32_t                m_vertexCount{ 0 };
	uint32_t                m_indicesCount{ 0 };
	GLuint                  m_vao{ 0 };
	GLuint                  m_vbo{ 0 };
	GLuint                  m_ibo{ 0 };
	std::optional<Material> m_material{};
	AABB                    m_aabb{};
};

class Model final
{
public:
	bool Load(const std::string& fileName, std::optional<glm::mat4> modelTransformMatrix = std::nullopt);
	void Create(const MeshCreateInfo& meshCreateInfo);
	void Create(const std::vector<MeshCreateInfo>& meshes);

	void Free();

	void DrawSubMesh(size_t id, GLenum mode = GL_TRIANGLES);
	void Draw(GLenum mode = GL_TRIANGLES);

	size_t GetNumMeshes() const { return m_meshes.size(); }
	const std::vector<Mesh>& GetMeshes() const { return m_meshes; }
	const Mesh& GetMesh(size_t id) const { return m_meshes[id]; }
	const AABB& GetAABB() const { return m_aabb; }

private:
	void processNode(const aiScene* scene, aiNode* node, std::string_view directory, std::optional<glm::mat4> modelTransformMatrix);
	Mesh processMesh(const aiScene* scene, struct aiMesh* mesh, std::string_view directory, std::optional<glm::mat4> modelTransformMatrix);
	void computeAABB();

	std::vector<Mesh> m_meshes;
	AABB              m_aabb;
};

namespace GeometryGenerator
{
	MeshCreateInfo CreatePlane(float width = 1.0f, float height = 1.0f, float wSegment = 1.0f, float hSegment = 1.0f);
	MeshCreateInfo CreateBox(float width = 1.0f, float height = 1.0f, float depth = 1.0, float widthSegments = 1.0f, float heightSegments = 1.0f, float depthSegments = 1.0f);
	MeshCreateInfo CreateSphere(float radius = 1.0f, float widthSegments = 8.0f, float heightSegments = 6.0f, float phiStart = 0.0f, float phiLength = M_PI * 2.0f, float thetaStart = 0.0f, float thetaLength = M_PI);
} // namespace GeometryGenerator