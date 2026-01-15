#include "stdafx.h"
#include "EditorCursor.h"
//=============================================================================
bool EditorCursor::Init()
{
	m_program = LoadShaderProgram("data/shaders/grid/vertex.glsl", "data/shaders/grid/fragment.glsl");
	if (!m_program.handle)
	{
		Fatal("Grid Shader failed!");
		return false;
	}
	glUseProgram(m_program.handle);

	//std::vector<float> vertices = {
	//	// передняя грань
	//	-0.5f, -0.5f,  0.5f,
	//	 0.5f, -0.5f,  0.5f,
	//	 0.5f,  0.5f,  0.5f,
	//	-0.5f,  0.5f,  0.5f,

	//	// задняя грань
	//	-0.5f, -0.5f, -0.5f,
	//	-0.5f,  0.5f, -0.5f,
	//	 0.5f,  0.5f, -0.5f,
	//	 0.5f, -0.5f, -0.5f,
	//};

	std::vector<float> vertices = {
		// передняя грань
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		// задняя грань
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	};
	std::vector<unsigned int> indices = {
		// передняя грань
		0, 1, 1, 2, 2, 3, 3, 0,
		// задняя грань
		4, 5, 5, 6, 6, 7, 7, 4,
		// соединительные ребра
		0, 4, 1, 7, 2, 6, 3, 5
	};

	GLuint currentVBO = GetCurrentBuffer(BufferTarget::Array);
	m_vbo = CreateBuffer(BufferTarget::Array, BufferUsage::StaticDraw, vertices.size() * sizeof(float), vertices.data());

	GLuint currentIBO = GetCurrentBuffer(BufferTarget::ElementArray);
	m_ibo = CreateBuffer(BufferTarget::ElementArray, BufferUsage::StaticDraw, indices.size() * sizeof(unsigned int), indices.data());
	m_indexCount = indices.size();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo.handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo.handle);
	VertexP3::SetVertexAttributes();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, currentVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentIBO);

	glUseProgram(0);

	return true;
}
//=============================================================================
void EditorCursor::Close()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_ibo.handle);
	glDeleteBuffers(1, &m_vbo.handle);
	glDeleteProgram(m_program.handle);
}
//=============================================================================
void EditorCursor::Draw(const glm::mat4& proj, const glm::mat4& view)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.5, 0.0, 0.5) + m_position);

	glDisable(GL_DEPTH_TEST);
	glUseProgram(m_program.handle);
	SetUniform(GetUniformLocation(m_program, "model"), model);
	SetUniform(GetUniformLocation(m_program, "view"), view);
	SetUniform(GetUniformLocation(m_program, "proj"), proj);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo.handle);
	glDrawElements(GL_LINES, m_indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
//=============================================================================
void EditorCursor::SetPosition(const glm::vec3& position)
{
	m_position = position;
}
//=============================================================================