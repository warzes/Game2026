#pragma once

#include "NanoRender.h"

class GridAxis final
{
public:
	GridAxis(int gridDim = 4);
	~GridAxis();

	void Draw(const glm::mat4& projection, const glm::mat4 view);

private:
	// Grid data
	GLuint m_vaoG;
	GLuint m_vboG;
	GLuint m_eboG;
	ProgramHandle m_gridShader;
	float* m_grid;
	int* m_indices;
	int m_dim;
	int m_nbPoints;
	int m_nbIndices;

	// Axis Data
	GLuint m_vaoA;
	GLuint m_vboA;
	ProgramHandle m_axisShader;
	float* m_axis;
};