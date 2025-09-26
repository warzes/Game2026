#include "stdafx.h"
#include "NanoRender.h"
#include "NanoCore.h"
#include "NanoLog.h"
#include "NanoIO.h"
//=============================================================================
struct TextureCache final
{
	bool operator==(const TextureCache&) const noexcept = default;

	std::string name;
	bool sRGB;
	bool flipVertical;
};
//=============================================================================
namespace std
{
	template<>
	struct hash<TextureCache>
	{
		std::size_t operator()(const TextureCache& tc) const noexcept
		{
			std::size_t h1 = std::hash<std::string>{}(tc.name);
			std::size_t h2 = std::hash<bool>{}(tc.sRGB);
			std::size_t h3 = std::hash<bool>{}(tc.flipVertical);
			std::size_t seed = 0;
			HashCombine(seed, h1, h2, h3);
			return seed;
		}
	};
}
//=============================================================================
namespace
{
	std::unordered_map<TextureCache, Texture2D> texturesMap;
	Texture2D defaultDiffuse2D;
	Texture2D defaultNormal2D;
	Texture2D defaultSpecular2D;
}
//=============================================================================
bool textures::Init()
{
	// Create default diffuse texture
	{
		constexpr size_t SizeTexture = 32u;
		uint8_t data[SizeTexture][SizeTexture][3];
		for (size_t i = 0; i < SizeTexture; i++)
		{
			for (size_t j = 0; j < SizeTexture; j++)
			{
				if ((i + j) % 2 == 0)
				{
					data[i][j][0] = 255;
					data[i][j][1] = 0;
					data[i][j][2] = 100;
				}
				else
				{
					data[i][j][0] = 100;
					data[i][j][1] = 0;
					data[i][j][2] = 255;
				}
			}
		}

		defaultDiffuse2D.id = CreateTexture2D(GL_SRGB8, SizeTexture, SizeTexture, GL_RGB, GL_UNSIGNED_BYTE, data);
		defaultDiffuse2D.width = SizeTexture;
		defaultDiffuse2D.height = SizeTexture;
	}

	// Create default normal texture
	{
		constexpr size_t SizeTexture = 8u;
		uint8_t data[SizeTexture][SizeTexture][3];
		for (size_t i = 0; i < SizeTexture; i++)
		{
			for (size_t j = 0; j < SizeTexture; j++)
			{
				data[i][j][0] = 128;
				data[i][j][1] = 128;
				data[i][j][2] = 255;
			}
		}

		defaultNormal2D.id = CreateTexture2D(GL_RGB8, SizeTexture, SizeTexture, GL_RGB, GL_UNSIGNED_BYTE, data);
		defaultNormal2D.width = SizeTexture;
		defaultNormal2D.height = SizeTexture;
	}

	// Create default specular texture
	{
		constexpr size_t SizeTexture = 8u;
		uint8_t data[SizeTexture][SizeTexture][3];
		for (size_t i = 0; i < SizeTexture; i++)
		{
			for (size_t j = 0; j < SizeTexture; j++)
			{
				data[i][j][0] = 255;//Roughness
				data[i][j][1] = 255;//Metallic
				data[i][j][2] = 0;
			}
		}

		defaultSpecular2D.id = CreateTexture2D(GL_RGB8, SizeTexture, SizeTexture, GL_RGB, GL_UNSIGNED_BYTE, data);
		defaultSpecular2D.width = SizeTexture;
		defaultSpecular2D.height = SizeTexture;
	}

	return true;
}
//=============================================================================
void textures::Close()
{
	glDeleteTextures(1, &defaultDiffuse2D.id); defaultDiffuse2D.id = 0;
	glDeleteTextures(1, &defaultNormal2D.id); defaultNormal2D.id = 0;
	glDeleteTextures(1, &defaultSpecular2D.id); defaultSpecular2D.id = 0;

	for (auto& it : texturesMap)
	{
		glDeleteTextures(1, &it.second.id);
	}
	texturesMap.clear();
}
//=============================================================================
Texture2D textures::GetDefaultDiffuse2D()
{
	return defaultDiffuse2D;
}
//=============================================================================
Texture2D textures::GetDefaultNormal2D()
{
	return defaultNormal2D;
}
//=============================================================================
Texture2D textures::GetDefaultSpecular2D()
{
	return defaultSpecular2D;
}
//=============================================================================
Texture2D textures::LoadTexture2D(const std::string& fileName, ColorSpace colorSpace, bool flipVertical)
{
	TextureCache keyMap = { .name = fileName, .sRGB = colorSpace == ColorSpace::sRGB, .flipVertical = flipVertical };
	auto it = texturesMap.find(keyMap);
	if (it != texturesMap.end())
	{
		return it->second;
	}
	else
	{
		bool hasTex = io::Exists(fileName);
		if (hasTex == false)
		{
			Error("Failed to load texture " + fileName);
			return GetDefaultDiffuse2D();
		}

		stbi_set_flip_vertically_on_load(flipVertical);

		int imgW, imgH, nrChannels;
		auto pixels = stbi_load(fileName.c_str(), &imgW, &imgH, &nrChannels, 0);
		if (!pixels || nrChannels < 1 || nrChannels > 4 || imgW < 0 || imgH < 0)
		{
			stbi_image_free(pixels);
			Error("Failed to load texture " + fileName);
			return GetDefaultDiffuse2D();
		}
		GLuint textureID{ 0 };
		glGenTextures(1, &textureID);

		GLenum internalFormat{ 0 };
		GLenum dataFormat{ 0 };
		if (nrChannels == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (nrChannels == 2)
		{
			internalFormat = dataFormat = GL_RG;
		}
		else if (nrChannels == 3)
		{
			internalFormat = colorSpace == ColorSpace::sRGB ? GL_SRGB8 : GL_RGB8;
			dataFormat = GL_RGB;
		}
		else if (nrChannels == 4)
		{
			internalFormat = colorSpace == ColorSpace::sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else
		{
			std::unreachable();
		}
		GLint currentTexture = GetCurrentTexture(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imgW, imgH, 0, dataFormat, GL_UNSIGNED_BYTE, pixels);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, currentTexture);

		stbi_image_free(pixels);

		Debug("Load Texture: " + fileName);

		texturesMap[keyMap] = Texture2D{ .id = textureID, .width = (uint32_t)imgW, .height = (uint32_t)imgH };
		return texturesMap[keyMap];
	}
}
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
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================
Mesh::Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices, std::optional<Material> material)
{
	assert(!vertices.empty());

	m_vertexCount = vertices.size();
	m_indicesCount = indices.size();

	GLuint currentVBO = GetCurrentBuffer(GL_ARRAY_BUFFER);
	GLuint currentIBO = GetCurrentBuffer(GL_ELEMENT_ARRAY_BUFFER);

	m_vbo = CreateStaticBuffer(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), vertices.data());
	if (!indices.empty())
		m_ibo = CreateStaticBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data());

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	if (m_ibo > 0) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	SetMeshVertexAttributes();
	glBindBuffer(GL_ARRAY_BUFFER, currentVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentIBO);
	glBindVertexArray(0);
}
//=============================================================================
Mesh::~Mesh()
{
	if (m_vbo) glDeleteBuffers(1, &m_vbo);
	if (m_ibo) glDeleteBuffers(1, &m_ibo);
	if (m_vao) glDeleteVertexArrays(1, &m_vao);
}
//=============================================================================
void Mesh::Draw(GLenum mode)
{
	assert(m_vao);

	glBindVertexArray(m_vao);
	if (m_ibo > 0)
	{
		glDrawElements(mode, m_indicesCount, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(mode, 0, m_vertexCount);
	}
	glBindVertexArray(0);
}
//=============================================================================
bool Model::Load(const std::string& fileName, std::optional<glm::mat4> modelTransformMatrix)
{
#define ASSIMP_LOAD_FLAGS (/*aiProcess_JoinIdenticalVertices |*/ \
                           aiProcess_Triangulate |               \
                           aiProcess_GenSmoothNormals |          \
                           /*aiProcess_LimitBoneWeights |     */ \
                           aiProcess_SplitLargeMeshes |          \
                           aiProcess_ImproveCacheLocality |      \
                           aiProcess_RemoveRedundantMaterials |  \
                           aiProcess_FindDegenerates |           \
                           aiProcess_FindInvalidData |           \
                           /*aiProcess_GenUVCoords |          */ \
                           aiProcess_FlipUVs |                   \
                           aiProcess_CalcTangentSpace |          \
                           aiProcess_SortByPType)

	Free();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName.c_str(), ASSIMP_LOAD_FLAGS);
	if (!scene || !scene->HasMeshes() || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Error("Not load mesh: " + fileName + "\n\tError: " + importer.GetErrorString());
		return false;
	}

	std::string directory = io::GetFileDirectory(fileName);
	processNode(scene, scene->mRootNode, directory, modelTransformMatrix);

	computeAABB();

	return true;
}
//=============================================================================
void Model::Create(const MeshCreateInfo& ci)
{
	Free();
	m_meshes.emplace_back(Mesh(ci.vertices, ci.indices, ci.material));
	computeAABB();
}
//=============================================================================
void Model::Create(const std::vector<MeshCreateInfo>& meshes)
{
	Free();
	for (size_t i = 0; i < meshes.size(); i++)
	{
		m_meshes.emplace_back(Mesh(meshes[i].vertices, meshes[i].indices, meshes[i].material));
	}
	computeAABB();
}
//=============================================================================
void Model::Free()
{
	m_meshes.clear();
}
//=============================================================================
void Model::DrawSubMesh(size_t id, GLenum mode)
{
	if (id < m_meshes.size())
		m_meshes[id].Draw(mode);
}
//=============================================================================
void Model::Draw(GLenum mode)
{
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].Draw(mode);
	}
}
//=============================================================================
void Model::processNode(const aiScene* scene, aiNode* node, std::string_view directory, std::optional<glm::mat4> modelTransformMatrix)
{
	for (unsigned i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.emplace_back(processMesh(scene, mesh, directory, modelTransformMatrix));
	}

	for (unsigned i = 0; i < node->mNumChildren; i++)
	{
		processNode(scene, node->mChildren[i], directory, modelTransformMatrix);
	}
}
//=============================================================================
Mesh Model::processMesh(const aiScene* scene, struct aiMesh* mesh, std::string_view directory, std::optional<glm::mat4> modelTransformMatrix)
{
	std::vector<MeshVertex> vertices(mesh->mNumVertices);
	std::vector<uint32_t> indices;
	indices.reserve(mesh->mNumFaces * 3);
	Material meshMaterial;

	// Process vertices
	for (unsigned i = 0; i < mesh->mNumVertices; i++)
	{
		MeshVertex& v = vertices[i];

		v.position.x = mesh->mVertices[i].x;
		v.position.y = mesh->mVertices[i].y;
		v.position.z = mesh->mVertices[i].z;
		if (modelTransformMatrix.has_value())
		{
			v.position = glm::vec3(modelTransformMatrix.value() * glm::vec4(v.position, 1.0f));
		}

		if (mesh->HasVertexColors(0))
		{
			v.color.x = mesh->mColors[0][i].r;
			v.color.y = mesh->mColors[0][i].g;
			v.color.z = mesh->mColors[0][i].b;
		}
		else
		{
			v.color = glm::vec3(1.0f);
		}

		if (mesh->HasNormals())
		{
			v.normal.x = mesh->mNormals[i].x;
			v.normal.y = mesh->mNormals[i].y;
			v.normal.z = mesh->mNormals[i].z;
			if (modelTransformMatrix.has_value())
			{
				const auto normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelTransformMatrix.value())));

				v.normal = glm::normalize(normalMatrix * v.normal);
			}
		}

		if (mesh->HasTextureCoords(0))
		{
			v.texCoord.x = mesh->mTextureCoords[0][i].x;
			v.texCoord.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			v.texCoord = glm::vec2(0.0f);
		}

		if (mesh->HasTangentsAndBitangents())
		{
			v.tangent.x = mesh->mTangents[i].x;
			v.tangent.y = mesh->mTangents[i].y;
			v.tangent.z = mesh->mTangents[i].z;
		}
		else
			v.tangent = glm::vec3(0.0f);
	}

	// Process indices
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		// Assume the model has only triangles.
		indices.emplace_back(mesh->mFaces[i].mIndices[0]);
		indices.emplace_back(mesh->mFaces[i].mIndices[1]);
		indices.emplace_back(mesh->mFaces[i].mIndices[2]);
	}

	// Process material
	{
		// TODO: по одной текстуре грузится, а тут есть возможность нескольих текстур
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		for (unsigned i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
		{
			aiString str;
			material->GetTexture(aiTextureType_DIFFUSE, i, &str);

			std::string path = directory.data();
			path += str.data;
			meshMaterial.diffuseTexture = textures::LoadTexture2D(path, ColorSpace::sRGB);
		}

		for (unsigned i = 0; i < material->GetTextureCount(aiTextureType_NORMALS); i++)
		{
			aiString str;
			material->GetTexture(aiTextureType_NORMALS, i, &str);

			std::string path = directory.data();
			path += str.data;
			meshMaterial.normalTexture = textures::LoadTexture2D(path, ColorSpace::Linear);
		}

		for (unsigned i = 0; i < material->GetTextureCount(aiTextureType_SPECULAR); i++)
		{
			aiString str;
			material->GetTexture(aiTextureType_SPECULAR, i, &str);

			std::string path = directory.data();
			path += str.data;
			meshMaterial.specularTexture = textures::LoadTexture2D(path, ColorSpace::Linear);
		}
	}

	return Mesh(vertices, indices, meshMaterial);
}
//=============================================================================
void Model::computeAABB()
{
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		m_aabb.CombineAABB(m_meshes[i].GetAABB());
	}
}
//=============================================================================
MeshCreateInfo GeometryGenerator::CreatePlane(float width, float height, float wSegment, float hSegment)
{
	MeshCreateInfo meshInfo;

	float width_half = width / 2.0f;
	float height_half = height / 2.0f;

	float gridX1 = wSegment + 1.0f;
	float gridY1 = hSegment + 1.0f;

	float segment_width = width / wSegment;
	float segment_height = height / hSegment;

	MeshVertex vertex;

	// generate Position Normal TexCoords
	for (int iy = 0; iy < static_cast<int>(gridY1); iy++)
	{
		float y = static_cast<float>(iy) * segment_height - height_half;

		for (int ix = 0; ix < static_cast<int>(gridX1); ix++)
		{
			float x = static_cast<float>(ix) * segment_width - width_half;
			vertex.position = glm::vec3(x, 0.0f, -y);
			vertex.color = glm::vec3(1.0f);
			vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			vertex.texCoord = glm::vec2(static_cast<float>(ix) / wSegment, 1.0f - (static_cast<float>(iy) / hSegment));
			vertex.tangent = glm::vec3(0.0f);
			meshInfo.vertices.push_back(vertex);
		}
	}
	// generate indices
	for (int iy = 0; iy < static_cast<int>(hSegment); iy++)
	{
		for (int ix = 0; ix < static_cast<int>(wSegment); ix++)
		{
			float a = static_cast<float>(ix) + gridX1 * static_cast<float>(iy);
			float b = static_cast<float>(ix) + gridX1 * static_cast<float>(iy + 1);
			float c = static_cast<float>(ix + 1) + gridX1 * static_cast<float>(iy + 1);
			float d = static_cast<float>(ix + 1) + gridX1 * static_cast<float>(iy);
			meshInfo.indices.push_back(static_cast<uint32_t>(a));
			meshInfo.indices.push_back(static_cast<uint32_t>(b));
			meshInfo.indices.push_back(static_cast<uint32_t>(d));
			meshInfo.indices.push_back(static_cast<uint32_t>(b));
			meshInfo.indices.push_back(static_cast<uint32_t>(c));
			meshInfo.indices.push_back(static_cast<uint32_t>(d));
		}
	}

	return meshInfo;
}
//=============================================================================
inline void buildBoxPlane(MeshCreateInfo& meshInfo, int& numberOfVertices, int u, int v, int w, float udir, float vdir, float width, float height, float depth, float gridX, float gridY)
{
	float segmentWidth = width / gridX;
	float segmentHeight = height / gridY;

	float widthHalf = width / 2.0f;
	float heightHalf = height / 2.0f;
	float depthHalf = depth / 2.0f;

	float gridX1 = gridX + 1.0f;
	float gridY1 = gridY + 1.0f;

	MeshVertex vertex;

	for (int iy = 0; iy < static_cast<int>(gridY1); iy++)
	{
		float y = static_cast<float>(iy) * segmentHeight - heightHalf;
		for (int ix = 0; ix < static_cast<int>(gridX1); ix++)
		{
			float x = static_cast<float>(ix) * segmentWidth - widthHalf;

			// position
			vertex.position[u] = x * udir;
			vertex.position[v] = y * vdir;
			vertex.position[w] = depthHalf;

			// normals
			vertex.normal[u] = 0.0f;
			vertex.normal[v] = 0.0f;
			vertex.normal[w] = depth > 0 ? 1.0f : -1.0f;

			// uvs
			vertex.texCoord = glm::vec2(static_cast<float>(ix) / gridX, 1.0f - static_cast<float>(iy) / gridY);

			meshInfo.vertices.push_back(vertex);
		}
	}

	// indices
	for (int iy = 0; iy < static_cast<int>(gridY); iy++)
	{
		for (int ix = 0; ix < static_cast<int>(gridX); ix++)
		{
			float a = static_cast<float>(numberOfVertices) + static_cast<float>(ix) + static_cast<float>(iy) * gridX1;
			float b = static_cast<float>(numberOfVertices) + static_cast<float>(ix) + static_cast<float>(iy + 1) * gridX1;
			float c = static_cast<float>(numberOfVertices) + static_cast<float>(ix + 1) + static_cast<float>(iy + 1) * gridX1;
			float d = static_cast<float>(numberOfVertices) + static_cast<float>(ix + 1) + static_cast<float>(iy) * gridX1;

			meshInfo.indices.push_back(static_cast<unsigned>(a));
			meshInfo.indices.push_back(static_cast<unsigned>(d));
			meshInfo.indices.push_back(static_cast<unsigned>(b));

			meshInfo.indices.push_back(static_cast<unsigned>(b));
			meshInfo.indices.push_back(static_cast<unsigned>(d));
			meshInfo.indices.push_back(static_cast<unsigned>(c));
		}
	}
	numberOfVertices += static_cast<int>(gridX1 * gridY1);
}
//=============================================================================
MeshCreateInfo GeometryGenerator::CreateBox(float width, float height, float depth, float widthSeg, float heightSeg, float depthSeg)
{
	MeshCreateInfo meshInfo;

	float widthSegments = static_cast<float>(std::max(1, static_cast<int>(std::floor(widthSeg))));
	float heightSegments = static_cast<float>(std::max(1, static_cast<int>(std::floor(heightSeg))));
	float depthSegments = static_cast<float>(std::max(1, static_cast<int>(std::floor(depthSeg))));

	int numberOfVertices = 0;

	// Плоскости: +X, -X, +Y, -Y, +Z, -Z
	buildBoxPlane(meshInfo, numberOfVertices, 2, 1, 0, -1, -1, depth, height, width, depthSegments, heightSegments); // +X
	buildBoxPlane(meshInfo, numberOfVertices, 2, 1, 0, 1, -1, depth, height, -width, depthSegments, heightSegments); // -X

	buildBoxPlane(meshInfo, numberOfVertices, 0, 2, 1, 1, 1, width, depth, height, widthSegments, depthSegments); // +Y
	buildBoxPlane(meshInfo, numberOfVertices, 0, 2, 1, 1, -1, width, depth, -height, widthSegments, depthSegments); // -Y

	buildBoxPlane(meshInfo, numberOfVertices, 0, 1, 2, 1, -1, width, height, depth, widthSegments, heightSegments); // +Z
	buildBoxPlane(meshInfo, numberOfVertices, 0, 1, 2, -1, -1, width, height, -depth, widthSegments, heightSegments); // -Z

	return meshInfo;
}
//=============================================================================
MeshCreateInfo GeometryGenerator::CreateSphere(float radius, float widthSeg, float heightSeg, float phiStart, float phiLength, float thetaStart, float thetaLength)
{
	MeshCreateInfo meshInfo;

	constexpr const float PI = glm::pi<float>();
	const float thetaEnd = glm::min(thetaStart + thetaLength, PI);

	float widthSegments = static_cast<float>(std::max(3u, static_cast<unsigned>(std::floor(widthSeg))));
	float heightSegments = static_cast<float>(std::max(2u, static_cast<unsigned>(std::floor(heightSeg))));

	std::vector<std::vector<unsigned>> grid;
	int index = 0;

	MeshVertex vertex;
	for (unsigned iy = 0; iy <= static_cast<unsigned>(heightSegments); iy++)
	{
		std::vector<unsigned> verticesRow;

		float v = static_cast<float>(iy) / heightSegments;

		float uOffset = 0.0f;

		// Коррекция UV на полюсах (для избежания сжатия)
		if (iy == 0 && thetaStart == 0.0f)
			uOffset = 0.5f / widthSegments;
		else if (iy == static_cast<unsigned>(heightSegments) && thetaEnd == PI)
			uOffset = -0.5f / widthSegments;

		for (unsigned ix = 0; ix <= static_cast<unsigned>(widthSegments); ix++)
		{
			const float u = static_cast<float>(ix) / widthSegments;

			const float phi = phiStart + u * phiLength;
			const float theta = thetaStart + v * thetaLength;

			// Позиция (Y — вверх, правосторонняя система) // TODO: переделать под левосторонюю?
			vertex.position.x = -radius * glm::cos(phi) * glm::sin(theta);
			vertex.position.y = radius * glm::cos(theta);
			vertex.position.z = radius * glm::sin(phi) * glm::sin(theta);

			// normal
			vertex.normal = glm::normalize(vertex.position);

			// uv
			vertex.texCoord = glm::vec2(u + uOffset, 1.0f - v);

			meshInfo.vertices.push_back(vertex);
			verticesRow.push_back(static_cast<unsigned>(index++));
		}
		grid.push_back(verticesRow);
	}

	// indices
	for (unsigned iy = 0; iy < static_cast<unsigned>(heightSegments); iy++)
	{
		for (unsigned ix = 0; ix < static_cast<unsigned>(widthSegments); ix++)
		{
			unsigned int a = grid[iy][ix + 1];
			unsigned int b = grid[iy][ix];
			unsigned int c = grid[iy + 1][ix];
			unsigned int d = grid[iy + 1][ix + 1];

			if (iy != 0 || thetaStart > 0)
			{
				meshInfo.indices.push_back(a);
				meshInfo.indices.push_back(d);
				meshInfo.indices.push_back(b);
			}

			if (iy != static_cast<unsigned>(heightSegments - 1) || thetaEnd < M_PI)
			{
				meshInfo.indices.push_back(b);
				meshInfo.indices.push_back(d);
				meshInfo.indices.push_back(c);
			}
		}
	}

	return meshInfo;
}
//=============================================================================