#pragma once

glm::mat4 GetTransformMatrix(const glm::vec3& position, const glm::vec3& rotation, float scale = 1.0f);

// Assuming you have a way to calculate normals, tangents, and bitangents.
// Here's a common approach using Newell's method for face normal
// and an arbitrary vector to calculate tangent and bitangent.
inline glm::vec3 CalculateFaceNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
	glm::vec3 edge1 = v1 - v0;
	glm::vec3 edge2 = v2 - v0;
	return glm::normalize(glm::cross(edge1, edge2));
}

inline void CalculateTangentBitangent(
	const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
	const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2,
	glm::vec3& tangent, glm::vec3& bitangent)
{

	glm::vec3 edge1 = v1 - v0;
	glm::vec3 edge2 = v2 - v0;

	glm::vec2 deltaUV1 = uv1 - uv0;
	glm::vec2 deltaUV2 = uv2 - uv0;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y + 1e-8f); // Epsilon to avoid division by zero

	tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
	bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

	tangent = glm::normalize(tangent);
	bitangent = glm::normalize(bitangent);
}


inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& m)
{
	glm::mat4 matrix;
	matrix[0][0] = m.a1; matrix[0][1] = m.b1; matrix[0][2] = m.c1; matrix[0][3] = m.d1;
	matrix[1][0] = m.a2; matrix[1][1] = m.b2; matrix[1][2] = m.c2; matrix[1][3] = m.d2;
	matrix[2][0] = m.a3; matrix[2][1] = m.b3; matrix[2][2] = m.c3; matrix[2][3] = m.d3;
	matrix[3][0] = m.a4; matrix[3][1] = m.b4; matrix[3][2] = m.c4; matrix[3][3] = m.d4;
	return matrix;
}

inline glm::mat3 assimpMat3_to_glmMat3(aiMatrix3x3& m)
{
	glm::mat3 matrix;
	matrix[0][0] = m.a1; matrix[1][0] = m.a2; matrix[2][0] = m.a3;
	matrix[0][1] = m.b1; matrix[1][1] = m.b2; matrix[2][1] = m.b3;
	matrix[0][2] = m.c1; matrix[1][2] = m.c2; matrix[2][2] = m.c3;
	return matrix;
}

class AABB final
{
public:
	AABB() = default;
	AABB(const glm::vec3& Min, const glm::vec3& Max)
		: min(glm::min(Min, Max))
		, max(glm::max(Min, Max))
	{
	}
	AABB(const glm::vec3* points, size_t numPoints)
	{
		glm::vec3 vmin(std::numeric_limits<float>::max());
		glm::vec3 vmax(std::numeric_limits<float>::lowest());

		for (size_t i = 0; i != numPoints; i++)
		{
			vmin = glm::min(vmin, points[i]);
			vmax = glm::max(vmax, points[i]);
		}
		min = vmin;
		max = vmax;
	}
	AABB(const std::vector<glm::vec3>& points)
		: AABB(points.data(), points.size())
	{
	}

	void Set(const std::vector<glm::vec3>& vertexData, const std::vector<uint32_t>& indexData);

	void CombinePoint(const glm::vec3& point)
	{
		min = glm::min(min, point);
		max = glm::max(max, point);
	}

	void CombineAABB(const AABB& aabb)
	{
		min = glm::min(min, aabb.min);
		max = glm::max(max, aabb.max);
	}

	bool Overlaps(const AABB& anotherAABB) const
	{
		return max.x > anotherAABB.min.x && min.x < anotherAABB.max.x
			&& max.y > anotherAABB.min.y && min.y < anotherAABB.max.y
			&& max.z > anotherAABB.min.z && min.z < anotherAABB.max.z;
	}

	bool Inside(const glm::vec3& point) const
	{
		return max.x > point.x && min.x < point.x
			&& max.y > point.y && min.y < point.y
			&& max.z > point.z && min.z < point.z;
	}

	void Transform(const glm::mat4& transform)
	{
		glm::vec3 corners[] =
		{
			glm::vec3(min.x, min.y, min.z),
			glm::vec3(min.x, max.y, min.z),
			glm::vec3(min.x, min.y, max.z),
			glm::vec3(min.x, max.y, max.z),
			glm::vec3(max.x, min.y, min.z),
			glm::vec3(max.x, max.y, min.z),
			glm::vec3(max.x, min.y, max.z),
			glm::vec3(max.x, max.y, max.z),
		};
		for (auto& v : corners)
			v = glm::vec3(transform * glm::vec4(v, 1.0f));
		*this = AABB(corners, 8);
	}
	AABB GetTransformed(const glm::mat4& t) const
	{
		AABB b = *this;
		b.Transform(t);
		return b;
	}

	float GetVolume() const
	{
		glm::vec3 Diagonal = max - min;
		return Diagonal.x * Diagonal.y * Diagonal.z;
	}

	glm::vec3 GetSize() const { return max - min; }
	glm::vec3 GetCenter() const { return (max + min) * 0.5f; }

	glm::vec3 min{ std::numeric_limits<float>::max() };
	glm::vec3 max{ -std::numeric_limits<float>::max() };
};

inline AABB CombineBoxes(const std::vector<AABB>& boxes)
{
	std::vector<glm::vec3> allPoints;
	allPoints.reserve(boxes.size() * 8);

	for (const auto& b : boxes)
	{
		allPoints.emplace_back(b.min.x, b.min.y, b.min.z);
		allPoints.emplace_back(b.min.x, b.min.y, b.max.z);
		allPoints.emplace_back(b.min.x, b.max.y, b.min.z);
		allPoints.emplace_back(b.min.x, b.max.y, b.max.z);

		allPoints.emplace_back(b.max.x, b.min.y, b.min.z);
		allPoints.emplace_back(b.max.x, b.min.y, b.max.z);
		allPoints.emplace_back(b.max.x, b.max.y, b.min.z);
		allPoints.emplace_back(b.max.x, b.max.y, b.max.z);
	}

	return AABB(allPoints.data(), allPoints.size());
}

inline bool IsBoxInFrustum(glm::vec4* frustumPlanes, glm::vec4* frustumCorners, const AABB& box)
{
	for (int i = 0; i < 6; i++)
	{
		int r = 0;
		r += (glm::dot(frustumPlanes[i], glm::vec4(box.min.x, box.min.y, box.min.z, 1.0f)) < 0.0) ? 1 : 0;
		r += (glm::dot(frustumPlanes[i], glm::vec4(box.max.x, box.min.y, box.min.z, 1.0f)) < 0.0) ? 1 : 0;
		r += (glm::dot(frustumPlanes[i], glm::vec4(box.min.x, box.max.y, box.min.z, 1.0f)) < 0.0) ? 1 : 0;
		r += (glm::dot(frustumPlanes[i], glm::vec4(box.max.x, box.max.y, box.min.z, 1.0f)) < 0.0) ? 1 : 0;
		r += (glm::dot(frustumPlanes[i], glm::vec4(box.min.x, box.min.y, box.max.z, 1.0f)) < 0.0) ? 1 : 0;
		r += (glm::dot(frustumPlanes[i], glm::vec4(box.max.x, box.min.y, box.max.z, 1.0f)) < 0.0) ? 1 : 0;
		r += (glm::dot(frustumPlanes[i], glm::vec4(box.min.x, box.max.y, box.max.z, 1.0f)) < 0.0) ? 1 : 0;
		r += (glm::dot(frustumPlanes[i], glm::vec4(box.max.x, box.max.y, box.max.z, 1.0f)) < 0.0) ? 1 : 0;
		if (r == 8)
			return false;
	}

	// check frustum outside/inside box
	int r = 0;
	r = 0;
	for (int i = 0; i < 8; i++)
		r += ((frustumCorners[i].x > box.max.x) ? 1 : 0);
	if (r == 8)
		return false;
	r = 0;
	for (int i = 0; i < 8; i++)
		r += ((frustumCorners[i].x < box.min.x) ? 1 : 0);
	if (r == 8)
		return false;
	r = 0;
	for (int i = 0; i < 8; i++)
		r += ((frustumCorners[i].y > box.max.y) ? 1 : 0);
	if (r == 8)
		return false;
	r = 0;
	for (int i = 0; i < 8; i++)
		r += ((frustumCorners[i].y < box.min.y) ? 1 : 0);
	if (r == 8)
		return false;
	r = 0;
	for (int i = 0; i < 8; i++)
		r += ((frustumCorners[i].z > box.max.z) ? 1 : 0);
	if (r == 8)
		return false;
	r = 0;
	for (int i = 0; i < 8; i++)
		r += ((frustumCorners[i].z < box.min.z) ? 1 : 0);
	if (r == 8)
		return false;

	return true;
}

inline void GetFrustumPlanes(glm::mat4 viewProj, glm::vec4* planes)
{
	viewProj = glm::transpose(viewProj);
	planes[0] = glm::vec4(viewProj[3] + viewProj[0]); // left
	planes[1] = glm::vec4(viewProj[3] - viewProj[0]); // right
	planes[2] = glm::vec4(viewProj[3] + viewProj[1]); // bottom
	planes[3] = glm::vec4(viewProj[3] - viewProj[1]); // top
	planes[4] = glm::vec4(viewProj[3] + viewProj[2]); // near
	planes[5] = glm::vec4(viewProj[3] - viewProj[2]); // far
}

inline void GetFrustumCorners(glm::mat4 viewProj, glm::vec4* points)
{
	const glm::vec4 corners[] = {
		glm::vec4(-1, -1, -1, 1), glm::vec4(1, -1, -1, 1), glm::vec4(1, 1, -1, 1), glm::vec4(-1, 1, -1, 1),
		glm::vec4(-1, -1, 1, 1),  glm::vec4(1, -1, 1, 1),  glm::vec4(1, 1, 1, 1),  glm::vec4(-1, 1, 1, 1) };

	const glm::mat4 invViewProj = glm::inverse(viewProj);

	for (int i = 0; i != 8; i++)
	{
		const glm::vec4 q = invViewProj * corners[i];
		points[i] = q / q.w;
	}
}

// Функция для получения луча из позиции курсора
inline glm::vec3 GetRayFromScreen(float screenX, float screenY, int screenWidth, int screenHeight, const glm::mat4& view, const glm::mat4& projection)
{
	// Нормализуем координаты экрана (-1 до 1)
	float x = (2.0f * screenX) / screenWidth - 1.0f;
	float y = 1.0f - (2.0f * screenY) / screenHeight; // Инвертируем y

	// Создаем векторы
	glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f); // Ближняя плоскость

	// Преобразуем в мировые координаты
	glm::mat4 invProjection = glm::inverse(projection);
	glm::mat4 invView = glm::inverse(view);

	glm::vec4 rayEye = invProjection * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // Устанавливаем w=0 для направления

	glm::vec4 rayWorld = invView * rayEye;

	// Нормализуем вектор направления
	glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z));

	return rayDir;
}