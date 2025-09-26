#pragma once

class Mesho2
{
public:
	Mesho2();

	void CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices);
	void RenderMesh();
	void ClearMesh();

	~Mesho2();

private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
};
