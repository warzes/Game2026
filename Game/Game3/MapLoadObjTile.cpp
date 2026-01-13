#include "stdafx.h"
#include "MapLoadObjTile.h"
//=============================================================================
// Структура для хранения данных модели
struct ObjModelData
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
};
//=============================================================================
// Глобальный кэш моделей
static std::unordered_map<std::string, ObjModelData> model_cache;
//=============================================================================
struct IndexLess
{
	bool operator()(const tinyobj::index_t& a, const tinyobj::index_t& b) const
	{
		if (a.vertex_index != b.vertex_index) return a.vertex_index < b.vertex_index;
		if (a.texcoord_index != b.texcoord_index) return a.texcoord_index < b.texcoord_index;
		return a.normal_index < b.normal_index;
	}
};
//=============================================================================
void ProcessModelData(const ObjModelData& model_data, const BlockModelInfo& modelInfo,
	std::vector<MeshVertex>& verticesWall, std::vector<unsigned int>& indicesWall,
	std::vector<MeshVertex>& verticesCeil, std::vector<unsigned int>& indicesCeil,
	std::vector<MeshVertex>& verticesFloor, std::vector<unsigned int>& indicesFloor)
{
	glm::mat4 rotation_matrix(1.0f);

	glm::mat4 rot_x(1.0f);
	if (modelInfo.rotate.x != 0.0f)
	{
		rot_x[1][1] = cosf(modelInfo.rotate.x);
		rot_x[1][2] = -sinf(modelInfo.rotate.x);
		rot_x[2][1] = sinf(modelInfo.rotate.x);
		rot_x[2][2] = cosf(modelInfo.rotate.x);
	}

	glm::mat4 rot_y(1.0f);
	if (modelInfo.rotate.y != 0.0f)
	{
		rot_y[0][0] = cosf(modelInfo.rotate.y);
		rot_y[0][2] = sinf(modelInfo.rotate.y);
		rot_y[2][0] = -sinf(modelInfo.rotate.y);
		rot_y[2][2] = cosf(modelInfo.rotate.y);
	}

	glm::mat4 rot_z(1.0f);
	if (modelInfo.rotate.z != 0.0f)
	{
		rot_z[0][0] = cosf(modelInfo.rotate.z);
		rot_z[0][1] = -sinf(modelInfo.rotate.z);
		rot_z[1][0] = sinf(modelInfo.rotate.z);
		rot_z[1][1] = cosf(modelInfo.rotate.z);
	}

	const auto& attrib = model_data.attrib;
	const auto& shapes = model_data.shapes;

	// Проходим по всем формам (мешам)
	for (size_t s = 0; s < shapes.size(); s++)
	{
		const auto& name = shapes[s].name;
		std::vector<MeshVertex>* vertices = nullptr;
		std::vector<unsigned int>* indices = nullptr;

		if (name == "bottom")
		{
			if (!modelInfo.bottomVisible) continue;

			vertices = &verticesCeil;
			indices = &indicesCeil;
		}
		else if (name == "top")
		{
			if (!modelInfo.topVisible) continue;

			vertices = &verticesFloor;
			indices = &indicesFloor;
		}
		else if (name == "left")
		{
			if (!modelInfo.leftVisible) continue;

			vertices = &verticesWall;
			indices = &indicesWall;
		}
		else if (name == "right")
		{
			if (!modelInfo.rightVisible) continue;

			vertices = &verticesWall;
			indices = &indicesWall;
		}
		else if (name == "forward")
		{
			if (!modelInfo.forwardVisible) continue;

			vertices = &verticesWall;
			indices = &indicesWall;
		}
		else if (name == "back")
		{
			if (!modelInfo.backVisible) continue;

			vertices = &verticesWall;
			indices = &indicesWall;
		}
		else
		{
			vertices = &verticesWall;
			indices = &indicesWall;
		}

		// Используем std::map с пользовательской функцией сравнения для tinyobj::index_t
		std::map<tinyobj::index_t, unsigned int, IndexLess> vertex_cache;

		for (size_t f = 0; f < shapes[s].mesh.indices.size(); f += 3)
		{
			// Получаем 3 индекса для треугольника
			tinyobj::index_t idx0 = shapes[s].mesh.indices[f + 0];
			tinyobj::index_t idx1 = shapes[s].mesh.indices[f + 1];
			tinyobj::index_t idx2 = shapes[s].mesh.indices[f + 2];

			// Инвертируем порядок, чтобы изменить winding (CW <-> CCW)
			// Было: idx0, idx1, idx2
			// Стало: idx0, idx2, idx1
			//const std::array<tinyobj::index_t, 3> reversed_indices = { idx0, idx2, idx1 };
			const std::array<tinyobj::index_t, 3> reversed_indices = { idx0, idx1, idx2 };

			for (const auto& idx : reversed_indices)
			{
				// Проверяем, была ли уже такая вершина обработана
				auto it = vertex_cache.find(idx);
				if (it == vertex_cache.end())
				{
					// Новая вершина
					MeshVertex vertex{};

					// Позиция
					if (idx.vertex_index >= 0 && static_cast<size_t>(idx.vertex_index) * 3 + 2 < attrib.vertices.size())
					{
						// Исходная позиция
						glm::vec3 pos = glm::vec3(
							attrib.vertices[3 * idx.vertex_index + 0],
							attrib.vertices[3 * idx.vertex_index + 1],
							attrib.vertices[3 * idx.vertex_index + 2]
						);

						rotation_matrix = glm::mat4(1.0f);
						if (modelInfo.rotate.x != 0.0f) // Вращение вокруг X (pitch)
							rotation_matrix = rot_x * rotation_matrix;
						if (modelInfo.rotate.y != 0.0f) // Вращение вокруг Y (yaw)
							rotation_matrix = rot_y * rotation_matrix;
						if (modelInfo.rotate.z != 0.0f) // Вращение вокруг Z (roll)
							rotation_matrix = rot_z * rotation_matrix;

						// Применяем вращение к позиции
						glm::vec4 rotated_pos = rotation_matrix * glm::vec4(pos, 1.0f);

						// Сдвигаем в позицию center
						vertex.position = glm::vec3(rotated_pos) + modelInfo.center;
					}
					else
					{
						vertex.position = modelInfo.center; // Если индекс неверный, используем центр
					}

					// Нормаль
					if (idx.normal_index >= 0 && static_cast<size_t>(idx.normal_index) * 3 + 2 < attrib.normals.size())
					{
						glm::vec3 normal = glm::vec3(
							attrib.normals[3 * idx.normal_index + 0],
							attrib.normals[3 * idx.normal_index + 1],
							attrib.normals[3 * idx.normal_index + 2]
						);
						// Применяем ту же матрицу вращения к нормали
						glm::vec4 rotated_normal = rotation_matrix * glm::vec4(normal, 0.0f);
						vertex.normal = glm::vec3(rotated_normal);

						// Нормализуем, если нужно
						vertex.normal = glm::normalize(vertex.normal);
					}

					// UV-координаты
					if (idx.texcoord_index >= 0 && static_cast<size_t>(idx.texcoord_index) * 2 + 1 < attrib.texcoords.size())
					{
						vertex.texCoord.x = attrib.texcoords[2 * idx.texcoord_index + 0];
						vertex.texCoord.y = attrib.texcoords[2 * idx.texcoord_index + 1];
					}

					// Цвет (установим по умолчанию, так как OBJ обычно не хранит цвет вершины)
					vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);

					// Тангент и битангент (установим по умолчанию, так как их нужно вычислять отдельно)
					vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
					vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);

					// Добавляем вершину в массив
					unsigned int new_index = static_cast<unsigned int>(vertices->size());
					vertices->push_back(vertex);

					// Сохраняем индекс новой вершины в кэше
					vertex_cache[idx] = new_index;

					// Добавляем индекс в массив индексов
					indices->push_back(new_index);
				}
				else
				{
					// Добавляем индекс из кэша
					indices->push_back(it->second);
				}
			}
		}
	}
}
//=============================================================================
void AddObjModel(const BlockModelInfo& modelInfo, 
	std::vector<MeshVertex>& verticesWall, std::vector<unsigned int>& indicesWall,
	std::vector<MeshVertex>& verticesCeil, std::vector<unsigned int>& indicesCeil,
	std::vector<MeshVertex>& verticesFloor, std::vector<unsigned int>& indicesFloor)
{
	// Проверяем, есть ли модель в кэше
	auto it = model_cache.find(modelInfo.modelPath);
	if (it != model_cache.end())
	{
		// Используем кэшированные данные
		const auto& cached_data = it->second;
		ProcessModelData(cached_data, modelInfo, verticesWall, indicesWall, verticesCeil, indicesCeil, verticesFloor, indicesFloor);
	}
	else
	{
		// Загружаем модель и сохраняем в кэш
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelInfo.modelPath.c_str());
		if (!ret)
		{
			Fatal("Error loading OBJ file: " + err);
			return;
		}
		if (!warn.empty())
		{
			Warning("TinyObjLoader Warning: " + warn);
		}

		// Сохраняем данные в кэш
		ObjModelData model_data;
		model_data.attrib = attrib;
		model_data.shapes = shapes;
		model_data.materials = materials;
		model_cache[modelInfo.modelPath] = model_data;

		ProcessModelData(model_data, modelInfo, verticesWall, indicesWall, verticesCeil, indicesCeil, verticesFloor, indicesFloor);
	}
}
//=============================================================================