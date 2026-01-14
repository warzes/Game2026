#pragma once

class MapGrid final
{
public:
	bool Init();
	void Close();

	void Draw(const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model);

private:
	ProgramHandle m_program{ 0 };
	GLuint        m_vao{ 0 };
	BufferHandle  m_vbo{ 0 };
	size_t        m_vertSize{ 0 };
};