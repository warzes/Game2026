#include "stdafx.h"
#include "GridAxis.h"
#include "NanoIO.h"
// TODO: сырые буферы заменить на Model
//=============================================================================
GridAxis::GridAxis(int gridDim)
{
	m_dim = gridDim;
	m_nbPoints = (m_dim + 1) * (m_dim + 1);
	m_nbIndices = 2 * (m_dim + 1) * (2 * m_dim);
	m_gridShader = CreateShaderProgram(LoadShaderCode("data/shaders/grid/vertex.glsl"), LoadShaderCode("data/shaders/grid/fragment.glsl"));
	m_axisShader = CreateShaderProgram(LoadShaderCode("data/shaders/axis/vertex.glsl"), LoadShaderCode("data/shaders/axis/fragment.glsl"));

	m_grid = new float[m_nbPoints * 3];
	m_indices = new int[m_nbIndices];

	for (int line{ 0 }; line < (m_dim + 1); ++line)
	{
		for (int col{ 0 }; col < (m_dim + 1); ++col)
		{
			int index = line * (m_dim + 1) + col;
			m_grid[(index * 3)] = static_cast<float>(-m_dim) / 2.0f + static_cast<float>(col);		// x
			m_grid[(index * 3) + 1] = 0.0f;															// y
			m_grid[(index * 3) + 2] = static_cast<float>(-m_dim) / 2.0f + static_cast<float>(line);	// z
		}
	}

	int a{ 0 }, b{ 1 };
	for (int i{ 0 }, j{ 1 }; i < (m_nbIndices / 2); i += 2, j += 2)
	{
		m_indices[i] = a++;
		m_indices[j] = b++;

		if (b % (m_dim + 1) == 0)
		{
			a++;
			b++;
		}
	}

	a = 0;
	b = m_dim + 1;
	int aInit = a;
	int bInit = b;
	int modulo = (m_dim + 1) * (m_dim + 1);
	for (int i{ m_nbIndices / 2 }, j{ (m_nbIndices / 2) + 1 }; i < m_nbIndices; i += 2, j += 2)
	{
		if (b % (modulo) == 0)
		{
			modulo++;
			a = ++aInit;
			b = ++bInit;
		}

		m_indices[i] = a;
		m_indices[j] = b;

		a += (m_dim + 1);
		b += (m_dim + 1);
	}

	glGenVertexArrays(1, &m_vaoG);
	glGenBuffers(1, &m_vboG);
	glGenBuffers(1, &m_eboG);

	glBindVertexArray(m_vaoG);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboG);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboG);

	glBufferData(GL_ARRAY_BUFFER, (m_nbPoints * 3) * sizeof(float), m_grid, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nbIndices * sizeof(int), m_indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	m_axis = new float[18] {
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, // x
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, // y
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f  // z
	};

	glGenVertexArrays(1, &m_vaoA);
	glGenBuffers(1, &m_vboA);

	glBindVertexArray(m_vaoA);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboA);

	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), m_axis, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	glUseProgram(m_gridShader.handle);
	SetUniform(GetUniformLocation(m_gridShader, "model"), glm::mat4(1.0f));

	glUseProgram(m_axisShader.handle);
	SetUniform(GetUniformLocation(m_axisShader, "model"), glm::mat4(1.0f));
}
//=============================================================================
GridAxis::~GridAxis()
{
	glDeleteProgram(m_gridShader.handle);
	glDeleteProgram(m_axisShader.handle);

	glBindVertexArray(m_vaoG);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &m_vboG);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &m_eboG);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &m_vaoG);

	glBindVertexArray(m_vaoA);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &m_vboA);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &m_vaoA);

	delete m_grid;
	delete m_indices;
	delete m_axis;
}
//=============================================================================
void GridAxis::Draw(const glm::mat4& projection, const glm::mat4 view)
{
	// start wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// draw grid
	glBindVertexArray(m_vaoG);
	glUseProgram(m_gridShader.handle);
	SetUniform(GetUniformLocation(m_gridShader, "view"), view);
	SetUniform(GetUniformLocation(m_gridShader, "proj"), projection);

	glDrawElements(GL_LINES, m_nbIndices, GL_UNSIGNED_INT, 0);

	// draw axis
	glBindVertexArray(m_vaoA);
	glUseProgram(m_axisShader.handle);
	//SetUniform(GetUniformLocation(m_axisShader, "model"), glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.01f, 0.0f)));
	SetUniform(GetUniformLocation(m_axisShader, "view"), view);
	SetUniform(GetUniformLocation(m_axisShader, "proj"), projection);

	SetUniform(GetUniformLocation(m_axisShader, "color"), glm::vec3(1.0f, 0.0f, 0.0f));
	glDrawArrays(GL_LINE_STRIP, 0, 2);
	SetUniform(GetUniformLocation(m_axisShader, "color"), glm::vec3(0.0f, 1.0f, 0.0f));
	glDrawArrays(GL_LINE_STRIP, 2, 2);
	SetUniform(GetUniformLocation(m_axisShader, "color"), glm::vec3(0.0f, 0.0f, 1.0f));
	glDrawArrays(GL_LINE_STRIP, 4, 2);

	// end wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
//=============================================================================