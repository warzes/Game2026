#pragma once

struct Entity;

class RPMainScene final
{
public:
	bool Init(uint16_t framebufferWidth, uint16_t framebufferHeight);
	void Close();

	void Resize(uint16_t framebufferWidth, uint16_t framebufferHeight);

	void Draw(const std::vector<Entity*>& entites, size_t numEntities);

private:
	void drawScene(const std::vector<Entity*>& entites, size_t numEntities);

	GLuint    m_program{ 0 };
	int       m_projectionMatrixId{ -1 };
	int       m_viewMatrixId{ -1 };
	int       m_modelMatrixId{ -1 };
	int       m_normalMatrixId{ -1 };
	uint16_t  m_framebufferWidth{ 0 };
	uint16_t  m_framebufferHeight{ 0 };
	glm::mat4 m_perspective{ 1.0f };

	std::unique_ptr<Framebuffer> m_fbo; // color + depth + stencil
};