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
	Texture2D CreateTextureFromData(std::string_view name, aiTexture* embTex, ColorSpace colorSpace = ColorSpace::Linear, bool flipVertical = false);
} // namespace textures

struct Material final
{
	float     opacity{ 1.0f };
	glm::vec3 diffuseColor{ 1.0f };
	glm::vec3 specularColor{ 1.0f };
	glm::vec3 ambientColor{ 1.0f };
	float     shininess{ 64.f };
	float     roughness{ 0.0f };
	float     metallic{ 0.0f };

	std::vector<Texture2D> diffuseTextures;
	std::vector<Texture2D> specularTextures;
	std::vector<Texture2D> normalTextures;
	std::vector<Texture2D> metallicRoughTextures;
	//std::vector<Texture2D> depthTextures;
	//std::vector<Texture2D> emissionTextures;

	bool noLighing{ false };
};

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

	void Draw(GLenum mode = GL_TRIANGLES, GLuint program = 0, bool bindMaterial = true, bool instancing = false, int amount = 1);

	uint32_t GetVertexCount() const noexcept { return m_vertexCount; }
	uint32_t GetIndexCount() const noexcept { return m_indicesCount; }
	std::optional<Material> GetMaterial() noexcept { return m_material; }
	const AABB& GetAABB() const noexcept { return m_aabb; }

private:
	uint32_t                m_vertexCount{ 0 };
	uint32_t                m_indicesCount{ 0 };
	GLuint                  m_vao{ 0 };
	GLuint                  m_vbo{ 0 };
	GLuint                  m_ebo{ 0 };
	std::optional<Material> m_material{};
	AABB                    m_aabb{};
};

struct ModelDrawInfo final
{
	GLenum mode{ GL_TRIANGLES };
	bool   bindMaterials{ true };
	GLuint shaderProgram{ 0 };
};

class Model final
{
public:
	bool Load(const std::string& fileName);
	void Create(const MeshCreateInfo& meshCreateInfo);
	void Create(const std::vector<MeshCreateInfo>& meshes);

	void Free();

	void DrawSubMesh(size_t id, GLenum mode = GL_TRIANGLES);
	void Draw(GLenum mode = GL_TRIANGLES);
	void Draw(const ModelDrawInfo& drawInfo);

	size_t GetNumMeshes() const noexcept { return m_meshes.size(); }
	const std::vector<Mesh>& GetMeshes() const noexcept { return m_meshes; }
	const Mesh& GetMesh(size_t id) const noexcept { return m_meshes[id]; }
	const AABB& GetAABB() const noexcept { return m_aabb; }

	bool Valid() const noexcept { return !m_meshes.empty(); }

private:
	void processNode(const aiScene* scene, aiNode* node, std::string_view directory);
	Mesh processMesh(const aiScene* scene, struct aiMesh* mesh, std::string_view directory);
	std::vector<Texture2D> loadMaterialTextures(std::string_view directory, const aiScene* scene, aiMaterial* mat, aiTextureType type, ColorSpace colorSpace);
	void computeAABB();

	std::vector<Mesh> m_meshes;
	AABB              m_aabb;
	std::string       m_name;
};

namespace GeometryGenerator
{
	MeshCreateInfo CreatePlane(float width = 1.0f, float height = 1.0f, float wSegment = 1.0f, float hSegment = 1.0f);
	MeshCreateInfo CreateBox(float width = 1.0f, float height = 1.0f, float depth = 1.0, float widthSegments = 1.0f, float heightSegments = 1.0f, float depthSegments = 1.0f);
	MeshCreateInfo CreateSphere(float radius = 1.0f, float widthSegments = 8.0f, float heightSegments = 6.0f, float phiStart = 0.0f, float phiLength = M_PI * 2.0f, float thetaStart = 0.0f, float thetaLength = M_PI);
} // namespace GeometryGenerator