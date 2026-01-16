#pragma once

class EditorCursor final
{
public:
	bool Init();
	void Close();

	void Draw(const glm::mat4& proj, const glm::mat4& view);

	void SetPosition(const glm::vec3& position);

private:
	ProgramHandle m_program{ 0 };
	GLuint        m_vao{ 0 };
	BufferHandle  m_vbo{ 0 };
	BufferHandle  m_ibo{ 0 };
	size_t        m_indexCount{ 0 };

	glm::vec3     m_position;
};

inline EditorCursor* gEditorCursor{ nullptr };