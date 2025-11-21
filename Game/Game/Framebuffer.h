#pragma once

#include "NanoOpenGL3Advance.h"

#pragma region [ NEW Framebuffer ]

// TODO: тест CubeMap fbo
// TODO: я использую GL_RGB32F, а можно сделать вариант чтобы можно было выбирать GL_RGB16F

enum class AttachmentType : uint8_t
{
	Texture,
	TextureCubeMap,
	RenderBuffer
};

struct ColorAttachment final
{
	AttachmentType type{ AttachmentType::Texture };
	ColorFormat    format{ ColorFormat::RGBA };
	DataType       dataType{ DataType::UnsignedByte };
	ColorSpace     colorSpace{ ColorSpace::Linear };
	bool           multisample{ false };
	int            samples{ 4 };
};

struct ColorAttachmentId final
{
	GLuint         id{ 0 };
	AttachmentType type{ AttachmentType::Texture };
};

struct DepthAttachmentId final
{
	GLuint         id{ 0 };
	AttachmentType type{ AttachmentType::Texture };
};

struct DepthAttachment final
{
	AttachmentType type{ AttachmentType::RenderBuffer };
	bool           multisample{ false };
	int            samples{ 4 };
};

struct FramebufferInfo final
{
	std::vector<ColorAttachment>   colorAttachments;
	std::optional<DepthAttachment> depthAttachment = std::nullopt;
	uint16_t                       width{ 1 };
	uint16_t                       height{ 1 };
};

class Framebuffer final
{
public:
	Framebuffer() = default;
	Framebuffer(Framebuffer&& other) noexcept;
	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(Framebuffer&& other) noexcept;
	Framebuffer& operator=(const Framebuffer&) = delete;

	bool Create(const FramebufferInfo& createInfo);
	void Destroy();

	void Bind();
	void BindOnlyDraw();
	void BindOnlyRead();
	static void Unbind();

	void Resize(uint16_t width, uint16_t height);

	GLuint GetId() const { return m_fbo; }

	void BindColorTexture(size_t colorAttachment, size_t slot) const;
	void BindDepthTexture(size_t slot) const;

private:
	bool initializeAttachments();
	void cleanupAttachments();

	void createColorTextureAttachment(size_t index, const ColorAttachment& cfg);
	void createColorCubeMapTextureAttachment(size_t index, const ColorAttachment& cfg);
	void createColorRenderbufferAttachment(size_t index, const ColorAttachment& cfg);

	void createDepthTextureAttachment(const DepthAttachment& cfg);
	void createDepthCubeMapTextureAttachment(const DepthAttachment& cfg);
	void createDepthRenderbufferAttachment(const DepthAttachment& cfg);

	GLenum getInternalFormat(ColorFormat format, DataType dataType, ColorSpace colorSpace);

	GLuint                           m_fbo{ 0 };
	FramebufferInfo                  m_info;
	std::vector<ColorAttachmentId>   m_colorAttachmentsId;
	std::optional<DepthAttachmentId> m_depthAttachmentId{};
};

#pragma endregion


#pragma region [ OLD Framebuffer ]

enum class tAttachmentTarget : uint8_t
{
	ColorRed,
	ColorRG,
	ColorRGB,
	ColorRGBA,
	Depth,
	Stencil,
	DepthStencil
};

struct tAttachment final
{
	GLuint           id;
	AttachmentType   type;
	tAttachmentTarget target;
};

class tFramebuffer final
{
public:
	tFramebuffer(bool renderColor = true, bool multiSample = false, bool hdr = false);
	~tFramebuffer();

	void AddAttachment(AttachmentType type, tAttachmentTarget target, int width, int height, int insertPos = -1);
	void UpdateAttachment(AttachmentType type, tAttachmentTarget target, int width, int height);

	void CreateDirectionalDepthFBO(int width, int height);
	void CreateOmnidirectionalDepthFBO(int width, int height);
	void CreateMultisampledFBO(int width, int height);
	void CreateResolveFBO(int width, int height);

	void UpdateDirectionalDepthFBO(int width, int height);
	void UpdateOmnidirectionalDepthFBO(int width, int height);
	void UpdateMultisampledFBO(int width, int height);
	void UpdateResolveFBO(int width, int height);

	std::vector<tAttachment>& GetAttachments();

	void Bind();
	void Unbind();

	void BlitFramebuffer(tFramebuffer& writeFBO, int width, int height);
	void BlitFramebuffer(std::unique_ptr<tFramebuffer>& writeFBO, int width, int height);

	GLuint GetId() const { return m_fbo; }

private:
	void addColorTextureAttachment(int width, int height, tAttachmentTarget target, int insertPos);
	void addDepthTextureAttachment(int width, int height, int insertPos);

	void addColorTextureCubemapAttachment(int width, int height, int insertPos);
	void addDepthTextureCubemapAttachment(int width, int height, int insertPos);

	void addColorRenderbufferAttachment(int width, int height, tAttachmentTarget target, int insertPos);
	void addDepthRenderbufferAttachment(int width, int height, int insertPos);
	void addDepthStencilRenderbufferAttachment(int width, int height, int insertPos);

	void updateColorTextureAttachment(int width, int height, tAttachmentTarget target, int insertPos);
	void updateDepthTextureAttachment(int width, int height, int insertPos);

	void updateColorTextureCubemapAttachment(int width, int height, int insertPos);
	void updateDepthTextureCubemapAttachment(int width, int height, int insertPos);

	void updateColorRenderbufferAttachment(int width, int height, tAttachmentTarget target, int insertPos);
	void updateDepthRenderbufferAttachment(int width, int height, int insertPos);
	void updateDepthStencilRenderbufferAttachment(int width, int height, int insertPos);

	GLuint                  m_fbo{ 0 };
	bool                    m_renderColor{ false };
	bool                    m_multiSample{ false };
	bool                    m_hdr{ false };
	std::vector<tAttachment> m_attachment;
	bool                    m_rawColor{ true };
};

#pragma endregion