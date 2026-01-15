#include "stdafx.h"
#include "MapGrid.h"
//=============================================================================
std::vector<float> generateGrid(float size, float step)
{
	std::vector<float> vertices;

	for (float i = -size; i <= size; i += step) {
		// Горизонтальные линии
		vertices.insert(vertices.end(), {
			-size+0.5f, 0.0f, i + 0.5f,
			 size+0.5f, 0.0f, i + 0.5f,
			});

		// Вертикальные линии
		vertices.insert(vertices.end(), {
			i+0.5f, 0.0f, -size + 0.5f,
			i+0.5f, 0.0f,  size + 0.5f,
			});
	}

	return vertices;
}
//=============================================================================
bool MapGrid::Init()
{
	m_program = LoadShaderProgram("data/shaders/grid/vertex.glsl", "data/shaders/grid/fragment.glsl");
	if (!m_program.handle)
	{
		Fatal("Grid Shader failed!");
		return false;
	}
	glUseProgram(m_program.handle);

	float gridSize = 100.0f;
	float gridStep = 1.0f;
	auto gridVertices = generateGrid(gridSize, gridStep);
	m_vertSize = gridVertices.size();

	GLuint currentVBO = GetCurrentBuffer(BufferTarget::Array);
	m_vbo = CreateBuffer(BufferTarget::Array, BufferUsage::StaticDraw, gridVertices.size() * sizeof(float), gridVertices.data());
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo.handle);
	VertexP3::SetVertexAttributes();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, currentVBO);

	glUseProgram(0);

	if (!m_cursor.Init())
		return false;

	return true;
}
//=============================================================================
void MapGrid::Close()
{
	m_cursor.Close();
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo.handle);
	glDeleteProgram(m_program.handle);
}
//=============================================================================
void MapGrid::Draw(const glm::mat4& proj, const glm::mat4& view)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(m_program.handle);
	SetUniform(GetUniformLocation(m_program, "model"), glm::mat4(1.0f));
	SetUniform(GetUniformLocation(m_program, "view"), view);
	SetUniform(GetUniformLocation(m_program, "proj"), proj);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_LINES, 0, m_vertSize / 3);
	glBindVertexArray(0);

	m_cursor.Draw(proj, view);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
//=============================================================================