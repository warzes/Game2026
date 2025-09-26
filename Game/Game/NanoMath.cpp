#include "stdafx.h"
#include "NanoMath.h"
//=============================================================================
void AABB::Set(const std::vector<glm::vec3>& vertexData, const std::vector<uint32_t>& indexData)
{
	if (indexData.size() > 0)
	{
		for (size_t index_id = 0; index_id < indexData.size(); index_id++)
		{
			CombinePoint(vertexData[indexData[index_id]]);
		}
	}
	else
	{
		for (size_t vertex_id = 0; vertex_id < vertexData.size(); vertex_id++)
		{
			CombinePoint(vertexData[vertex_id]);
		}
	}
}
//=============================================================================