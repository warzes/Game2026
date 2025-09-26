#pragma once


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