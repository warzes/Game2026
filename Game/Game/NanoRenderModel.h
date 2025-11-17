#pragma once

#include "NanoRenderMesh.h"

struct ModelDrawInfo final
{
	GLenum mode{ GL_TRIANGLES };
	bool   bindMaterials{ true };
	ProgramHandle shaderProgram{};
};

enum class ModelMaterialType : uint8_t
{
	None,
	BlinnPhong,
	PBR
};

class Model final
{
public:
	bool Load(const std::string& fileName, ModelMaterialType materialType);
	void Create(const MeshInfo& meshCreateInfo);
	void Create(const std::vector<MeshInfo>& meshes);

	void Free();

	void DrawSubMesh(size_t id, GLenum mode = GL_TRIANGLES);
	void tDraw(GLenum mode = GL_TRIANGLES);
	void tDraw(const ModelDrawInfo& drawInfo);

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
	ModelMaterialType m_materialType;
	AABB              m_aabb;
	std::string       m_name;
};