#include "stdafx.h"
#include "Framebuffer.h"
#include "NanoLog.h"

#pragma region [ NEW Framebuffer ]
//=============================================================================
bool Framebuffer::Create(const FramebufferInfo& createInfo)
{
	Destroy();

	m_info = createInfo;

	glGenFramebuffers(1, &m_fbo);
	if (!initializeAttachments())
		return false;
	return true;
}
//=============================================================================
void Framebuffer::Destroy()
{
	if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
	m_fbo = 0;
	cleanupAttachments();
	m_info.colorAttachments.clear();
	m_info.depthAttachment = {};
	m_info.width = m_info.height = 0;
}
//=============================================================================
void Framebuffer::Bind()
{
	assert(m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}
//=============================================================================
void Framebuffer::BindOnlyDraw()
{
	assert(m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}
//=============================================================================
void Framebuffer::BindOnlyRead()
{
	assert(m_fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
}
//=============================================================================
void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::Resize(uint16_t width, uint16_t height)
{
	if (m_info.width == width && m_info.height == height) return;

	m_info.width = width;
	m_info.height = height;
	cleanupAttachments();
	initializeAttachments();
}
//=============================================================================
void Framebuffer::BindColorTexture(size_t colorAttachment, size_t slot) const
{
	if (colorAttachment < m_colorAttachmentsId.size())
	{
		if (m_colorAttachmentsId[colorAttachment].type == AttachmentType::Texture)
		{
			BindTexture2D(slot, m_colorAttachmentsId[colorAttachment].id);
		}
		else
		{
			// TODO:
		}
	}
	else
	{
		Error("colorAttachment not find");
	}
}
//=============================================================================
void Framebuffer::BindDepthTexture(size_t slot) const
{
	if (m_depthAttachmentId && m_depthAttachmentId->id)
	{
		if (m_depthAttachmentId->type == AttachmentType::Texture)
		{
			BindTexture2D(slot, m_depthAttachmentId->id);
		}
		else
		{
			// TODO:
		}
	}
	else
	{
		Error("depthAttachment not find");
	}
}
//=============================================================================
bool Framebuffer::initializeAttachments()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// color attachments
	std::vector<GLenum> drawBuffers;
	for (size_t i = 0; i < m_info.colorAttachments.size(); ++i)
	{
		const auto& cfg = m_info.colorAttachments[i];
		if (cfg.type == AttachmentType::Texture)
		{
			createColorTextureAttachment(i, cfg);
		}
		else if (cfg.type == AttachmentType::TextureCubeMap)
		{
			createColorCubeMapTextureAttachment(i, cfg);
		}
		else if (cfg.type == AttachmentType::RenderBuffer)
		{
			createColorRenderbufferAttachment(i, cfg);
		}
		else
		{
			continue;
		}
		drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i));
	}
	if (drawBuffers.empty())
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	else
	{
		glDrawBuffers(drawBuffers.size(), drawBuffers.data());
	}

	// depth buffer attachment
	if (m_info.depthAttachment)
	{
		const auto& depthCfg = m_info.depthAttachment.value();
		if (depthCfg.type == AttachmentType::Texture)
		{
			createDepthTextureAttachment(depthCfg);
		}
		else if (depthCfg.type == AttachmentType::TextureCubeMap)
		{
			createDepthCubeMapTextureAttachment(depthCfg);
		}
		else if (depthCfg.type == AttachmentType::RenderBuffer)
		{
			createDepthRenderbufferAttachment(depthCfg);
		}
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (status)
		{
		case GL_FRAMEBUFFER_UNDEFINED:
			Fatal("Framebuffer is not complete after resize: GL_FRAMEBUFFER_UNDEFINED");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			Fatal("Framebuffer is not complete after resize: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			Fatal("Framebuffer is not complete after resize: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			Fatal("Framebuffer is not complete after resize: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			Fatal("Framebuffer is not complete after resize: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			Fatal("Framebuffer is not complete after resize: GL_FRAMEBUFFER_UNSUPPORTED");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			Fatal("Framebuffer is not complete after resize: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			Fatal("Framebuffer is not complete after resize: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
			break;
		}

		Destroy();
		return false;
	}

	return true;
}
//=============================================================================
void Framebuffer::cleanupAttachments()
{
	for (auto cls : m_colorAttachmentsId)
	{
		if (cls.id)
		{
			if (cls.type == AttachmentType::Texture || cls.type == AttachmentType::TextureCubeMap)
				glDeleteTextures(1, &cls.id);
			else if(cls.type == AttachmentType::RenderBuffer)
				glDeleteRenderbuffers(1, &cls.id);
		}
		cls.id = 0;
	}
	if (m_depthAttachmentId)
	{
		if (m_depthAttachmentId->id)
		{
			if (m_depthAttachmentId->type == AttachmentType::Texture || m_depthAttachmentId->type == AttachmentType::TextureCubeMap)
				glDeleteTextures(1, &m_depthAttachmentId->id);
			else if (m_depthAttachmentId->type == AttachmentType::RenderBuffer)			
				glDeleteRenderbuffers(1, &m_depthAttachmentId->id);
		}
		m_depthAttachmentId->id = 0;
	}
	m_colorAttachmentsId.clear();
	m_depthAttachmentId = {};
}
//=============================================================================
void Framebuffer::createColorTextureAttachment(size_t index, const ColorAttachment& cfg)
{
	GLuint tex{ 0 };
	glGenTextures(1, &tex);
	assert(tex);
	if (cfg.multisample)
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, cfg.samples, getInternalFormat(cfg.format, cfg.dataType, cfg.colorSpace), m_info.width, m_info.height, GL_TRUE);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, getInternalFormat(cfg.format, cfg.dataType, cfg.colorSpace), m_info.width, m_info.height, 0, GetColorFormatGL(cfg.format), GetDataTypeGL(cfg.dataType), nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, (cfg.multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D), tex, 0);

	m_colorAttachmentsId.emplace_back(ColorAttachmentId{.id = tex, .type = cfg.type });
}
//=============================================================================
void Framebuffer::createColorCubeMapTextureAttachment(size_t index, const ColorAttachment& cfg)
{
	if (cfg.multisample)
	{
		Fatal("Multisample cube map textures are not supported in OpenGL 3.3");
		return;
	}
	GLuint tex{ 0 };
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	for (int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, getInternalFormat(cfg.format, cfg.dataType, cfg.colorSpace), m_info.width, m_info.height, 0, GetColorFormatGL(cfg.format), GetDataTypeGL(cfg.dataType), nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_CUBE_MAP_POSITIVE_X, tex, 0);
	m_colorAttachmentsId.emplace_back(ColorAttachmentId{ .id = tex, .type = cfg.type });
}
//=============================================================================
void Framebuffer::createColorRenderbufferAttachment(size_t index, const ColorAttachment& cfg)
{
	GLuint rb{ 0 };
	glGenRenderbuffers(1, &rb);
	glBindRenderbuffer(GL_RENDERBUFFER, rb);
	if (cfg.multisample)
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, cfg.samples, getInternalFormat(cfg.format, cfg.dataType, cfg.colorSpace), m_info.width, m_info.height);
	}
	else
	{
		glRenderbufferStorage(GL_RENDERBUFFER, getInternalFormat(cfg.format, cfg.dataType, cfg.colorSpace), m_info.width, m_info.height);
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, rb);
	m_colorAttachmentsId.emplace_back(ColorAttachmentId{ .id = rb, .type = cfg.type });
}
//=============================================================================
void Framebuffer::createDepthTextureAttachment(const DepthAttachment& cfg)
{
	GLuint tex = 0;
	if (cfg.multisample)
	{
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, cfg.samples, GL_DEPTH_COMPONENT32, m_info.width, m_info.height, GL_TRUE);
	}
	else
	{
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, m_info.width, m_info.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, (cfg.multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D), tex, 0);
	m_depthAttachmentId = DepthAttachmentId{ .id = tex, .type = cfg.type };
}
//=============================================================================
void Framebuffer::createDepthCubeMapTextureAttachment(const DepthAttachment& cfg)
{
	if (cfg.multisample)
	{
		Fatal("Multisample cube map depth textures are not supported in OpenGL 3.3");
		return;
	}
	GLuint tex{ 0 };
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32, m_info.width, m_info.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, tex, 0);
	m_depthAttachmentId = DepthAttachmentId{ .id = tex, .type = cfg.type };
}
//=============================================================================
void Framebuffer::createDepthRenderbufferAttachment(const DepthAttachment& cfg)
{
	GLuint rb = 0;
	glGenRenderbuffers(1, &rb);
	glBindRenderbuffer(GL_RENDERBUFFER, rb);
	if (cfg.multisample)
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, cfg.samples, GL_DEPTH_COMPONENT32, m_info.width, m_info.height);
	}
	else
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, m_info.width, m_info.height);
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);
	m_depthAttachmentId = DepthAttachmentId{ .id = rb, .type = cfg.type };
}
//=============================================================================
GLenum Framebuffer::getInternalFormat(ColorFormat format, DataType dataType, ColorSpace colorSpace)
{
	if (colorSpace == ColorSpace::sRGB && dataType == DataType::Float)
	{
		Fatal("sRGB is not supported with float data type in OpenGL 3.3");
		return 0;
	}

	switch (format)
	{
	case ColorFormat::Red:  return (dataType == DataType::Float) ? GL_R32F : GL_R8;
	case ColorFormat::RG:   return (dataType == DataType::Float) ? GL_RG32F : GL_RG8;

	case ColorFormat::RGB:  return (dataType == DataType::Float) 
		? GL_RGB32F 
		: (colorSpace == ColorSpace::sRGB ? GL_SRGB8 : GL_RGB8);

	case ColorFormat::RGBA: return (dataType == DataType::Float) 
		? GL_RGBA32F 
		: (colorSpace == ColorSpace::sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8);

	default:
		Fatal("Format not supported for this color format");
	}

	return 0;
}
//=============================================================================
#pragma endregion


#pragma region [ OLD Framebuffer ]


// TODO: отрефакторить
// после создания текстуры вернуть бинд старой
// TODO: в updateColorTextureAttachment и деструкторе отцепляется только GL_COLOR_ATTACHMENT0, если несколько текстур то проблема
//=============================================================================
tFramebuffer::tFramebuffer(bool color, bool ms, bool hdr)
	: m_renderColor(color)
	, m_multiSample(ms)
	, m_hdr(hdr)
{
	glGenFramebuffers(1, &m_fbo);
}
//=============================================================================
tFramebuffer::~tFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	for (int i{ 0 }; i < m_attachment.size(); ++i)
	{
		if (m_attachment.at(i).type == AttachmentType::Texture)
		{
			switch (m_attachment.at(i).target)
			{
			case tAttachmentTarget::ColorRed:
			case tAttachmentTarget::ColorRG:
			case tAttachmentTarget::ColorRGB:
			case tAttachmentTarget::ColorRGBA:
				if (m_multiSample)
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
				else
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
				break;
			case tAttachmentTarget::Depth:
				if (m_multiSample)
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
				else
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
				break;
			case tAttachmentTarget::Stencil:
				if (m_multiSample)
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
				else
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
				break;
			default:
				break;
			}
			glDeleteTextures(1, &m_attachment.at(i).id);
		}
		else if (m_attachment.at(i).type == AttachmentType::TextureCubeMap)
		{
			switch (m_attachment.at(i).target)
			{
			case tAttachmentTarget::ColorRed:
			case tAttachmentTarget::ColorRG:
			case tAttachmentTarget::ColorRGB:
			case tAttachmentTarget::ColorRGBA:
				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
				break;
			case tAttachmentTarget::Depth:
				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
				break;
			default:
				break;
			}
			glDeleteTextures(1, &m_attachment.at(i).id);
		}
		else if (m_attachment.at(i).type == AttachmentType::RenderBuffer)
		{
			switch (m_attachment.at(i).target)
			{
			case tAttachmentTarget::ColorRed:
			case tAttachmentTarget::ColorRG:
			case tAttachmentTarget::ColorRGB:
			case tAttachmentTarget::ColorRGBA:
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
				break;
			case tAttachmentTarget::Depth:
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
				break;
			case tAttachmentTarget::Stencil:
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
				break;
			case tAttachmentTarget::DepthStencil:
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
				break;
			default:
				break;
			}
			glDeleteRenderbuffers(1, &m_attachment.at(i).id);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &m_fbo);
}
//=============================================================================
void tFramebuffer::AddAttachment(AttachmentType type, tAttachmentTarget target, int width, int height, int insertPos)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	if (type == AttachmentType::Texture)
	{
		switch (target)
		{
		case tAttachmentTarget::ColorRed:
		case tAttachmentTarget::ColorRG:
		case tAttachmentTarget::ColorRGB:
		case tAttachmentTarget::ColorRGBA:
			addColorTextureAttachment(width, height, target, insertPos);
			break;
		case tAttachmentTarget::Depth:
			addDepthTextureAttachment(width, height, insertPos);
			break;
		default: break;
		}
	}
	else if (type == AttachmentType::TextureCubeMap)
	{
		switch (target)
		{
		case tAttachmentTarget::ColorRed:
		case tAttachmentTarget::ColorRG:
		case tAttachmentTarget::ColorRGB:
		case tAttachmentTarget::ColorRGBA:
			addColorTextureCubemapAttachment(width, height, insertPos);
			break;
		case tAttachmentTarget::Depth:
			addDepthTextureCubemapAttachment(width, height, insertPos);
			break;
		default: break;
		}
	}
	else if (type == AttachmentType::RenderBuffer)
	{
		switch (target)
		{
		case tAttachmentTarget::ColorRed:
		case tAttachmentTarget::ColorRG:
		case tAttachmentTarget::ColorRGB:
		case tAttachmentTarget::ColorRGBA:
			addColorRenderbufferAttachment(width, height, target, insertPos);
			break;
		case tAttachmentTarget::Depth:
			addDepthRenderbufferAttachment(width, height, insertPos);
			break;
		case tAttachmentTarget::DepthStencil:
			addDepthStencilRenderbufferAttachment(width, height, insertPos);
			break;
		default: break;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void tFramebuffer::UpdateAttachment(AttachmentType type, tAttachmentTarget target, int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	for (int i{ 0 }; i < m_attachment.size(); ++i)
	{
		if (m_attachment.at(i).type == type && m_attachment.at(i).target == target)
		{
			if (type == AttachmentType::Texture)
			{
				switch (target)
				{
				case tAttachmentTarget::ColorRed:
				case tAttachmentTarget::ColorRG:
				case tAttachmentTarget::ColorRGB:
				case tAttachmentTarget::ColorRGBA:
					updateColorTextureAttachment(width, height, target, i);
					break;
				case tAttachmentTarget::Depth:
					updateDepthTextureAttachment(width, height, i);
					break;
				default: break;
				}
			}
			else if (type == AttachmentType::TextureCubeMap)
			{
				switch (target)
				{
				case tAttachmentTarget::ColorRed:
				case tAttachmentTarget::ColorRG:
				case tAttachmentTarget::ColorRGB:
				case tAttachmentTarget::ColorRGBA:
					updateColorTextureCubemapAttachment(width, height, i);
					break;
				case tAttachmentTarget::Depth:
					updateDepthTextureCubemapAttachment(width, height, i);
					break;
				default: break;
				}
			}
			else if (type == AttachmentType::RenderBuffer)
			{
				switch (target)
				{
				case tAttachmentTarget::ColorRed:
				case tAttachmentTarget::ColorRG:
				case tAttachmentTarget::ColorRGB:
				case tAttachmentTarget::ColorRGBA:
					updateColorRenderbufferAttachment(width, height, target, i);
					break;
				case tAttachmentTarget::Depth:
					updateDepthRenderbufferAttachment(width, height, i);
					break;
				case tAttachmentTarget::DepthStencil:
					updateDepthStencilRenderbufferAttachment(width, height, i);
					break;
				default: break;
				}
			}
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void tFramebuffer::CreateDirectionalDepthFBO(int width, int height)
{
	m_multiSample = false;
	m_renderColor = false;
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	tAttachment buffer;
	buffer.type = AttachmentType::Texture;
	buffer.target = tAttachmentTarget::Depth;

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &buffer.id);
	glBindTexture(GL_TEXTURE_2D, buffer.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer.id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
		m_attachment.push_back(buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
//=============================================================================
void tFramebuffer::CreateOmnidirectionalDepthFBO(int width, int height)
{
	m_multiSample = false;
	m_renderColor = false;
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	tAttachment buffer;
	buffer.type = AttachmentType::TextureCubeMap;
	buffer.target = tAttachmentTarget::Depth;

	glGenTextures(1, &buffer.id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, buffer.id);
	for (int i{ 0 }; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, buffer.id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
		m_attachment.push_back(buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void tFramebuffer::CreateMultisampledFBO(int width, int height)
{
	m_multiSample = true;
	m_renderColor = true;
	struct tAttachment bufferColor;
	bufferColor.type = AttachmentType::Texture;
	bufferColor.target = tAttachmentTarget::ColorRGBA;

	tAttachment bufferDS;
	bufferDS.type = AttachmentType::RenderBuffer;
	bufferDS.target = tAttachmentTarget::DepthStencil;

	glGenTextures(1, &bufferColor.id);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, bufferColor.id);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, width, height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glGenRenderbuffers(1, &bufferDS.id);
	glBindRenderbuffer(GL_RENDERBUFFER, bufferDS.id);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, bufferColor.id, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, bufferDS.id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
	{
		m_attachment.push_back(bufferColor);
		m_attachment.push_back(bufferDS);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void tFramebuffer::CreateResolveFBO(int width, int height)
{
	m_multiSample = false;
	m_renderColor = true;

	tAttachment buffer;
	buffer.type = AttachmentType::Texture;
	buffer.target = tAttachmentTarget::ColorRGBA;

	glGenTextures(1, &buffer.id);
	glBindTexture(GL_TEXTURE_2D, buffer.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.id, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
		m_attachment.push_back(buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}
//=============================================================================
void tFramebuffer::UpdateDirectionalDepthFBO(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	glDeleteTextures(1, &m_attachment.at(0).id);
	m_attachment.clear();
	CreateDirectionalDepthFBO(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void tFramebuffer::UpdateOmnidirectionalDepthFBO(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
	glDeleteTextures(1, &m_attachment.at(0).id);
	m_attachment.clear();
	CreateOmnidirectionalDepthFBO(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void tFramebuffer::UpdateMultisampledFBO(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
	glDeleteTextures(1, &m_attachment.at(0).id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
	glDeleteRenderbuffers(1, &m_attachment.at(1).id);
	m_attachment.clear();
	CreateMultisampledFBO(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void tFramebuffer::UpdateResolveFBO(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glDeleteTextures(1, &m_attachment.at(0).id);
	m_attachment.clear();
	CreateResolveFBO(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
std::vector<tAttachment>& tFramebuffer::GetAttachments()
{
	return m_attachment;
}
//=============================================================================
void tFramebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	if (m_rawColor)
	{
		size_t colorsNum = 0;
		for (size_t i = 0; i < m_attachment.size(); i++)
		{
			if (m_attachment[i].type == AttachmentType::Texture && (m_attachment[i].target == tAttachmentTarget::ColorRed || m_attachment[i].target == tAttachmentTarget::ColorRG || m_attachment[i].target == tAttachmentTarget::ColorRGB || m_attachment[i].target == tAttachmentTarget::ColorRGBA))
			{
				colorsNum++;
			}
		}
		if (colorsNum > 1)
		{
			std::vector<GLuint> attachments(colorsNum);
			for (size_t i = 0; i < attachments.size(); i++)
			{
				attachments[i] = GL_COLOR_ATTACHMENT0 + i;
			}
			glDrawBuffers(colorsNum, attachments.data());
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			Error("framebuffer is not complete !");

		m_rawColor = false;
	}
}
//=============================================================================
void tFramebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void tFramebuffer::BlitFramebuffer(tFramebuffer& writeFBO, int width, int height)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, writeFBO.GetId());
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
//=============================================================================
void tFramebuffer::BlitFramebuffer(std::unique_ptr<tFramebuffer>& writeFBO, int width, int height)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, writeFBO->GetId());
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
//=============================================================================
void tFramebuffer::addColorTextureAttachment(int width, int height, tAttachmentTarget target, int insertPos)
{
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	tAttachment buffer;
	buffer.type = AttachmentType::Texture;
	buffer.target = target;
	GLint internalFormat = (m_hdr) ? GL_RGBA16F : GL_RGBA;
	GLenum format = GL_RGBA;
	if (target == tAttachmentTarget::ColorRed)
	{
		internalFormat = GL_RED; // TODO: hdr?
		format = GL_RED;
	}
	else if (target == tAttachmentTarget::ColorRG)
	{
		internalFormat = (m_hdr) ? GL_RG16F : GL_RG;
		format = GL_RG;
	}
	else if (target == tAttachmentTarget::ColorRGB)
	{
		internalFormat = (m_hdr) ? GL_RGB16F : GL_RGB;
		format = GL_RGB;
	}

	GLenum type = (m_hdr) ? GL_FLOAT : GL_UNSIGNED_BYTE;

	int id = 0;
	if (insertPos == -1)
		id = m_attachment.size();
	else
		id = insertPos;

	if (m_multiSample)
	{
		glGenTextures(1, &buffer.id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, buffer.id);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, internalFormat, width, height, GL_TRUE);
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // TODO: выдает ошибку
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // TODO: выдает ошибку
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, buffer.id, 0);
	}
	else
	{
		glGenTextures(1, &buffer.id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, buffer.id);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + id, GL_TEXTURE_2D, buffer.id, 0);
	}

	if (insertPos == -1)
		m_attachment.push_back(buffer);
	else
		m_attachment.insert(m_attachment.begin() + insertPos, buffer);

	m_rawColor = true;
}
//=============================================================================
void tFramebuffer::addDepthTextureAttachment(int width, int height, int insertPos)
{
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	tAttachment buffer;
	buffer.type = AttachmentType::Texture;
	buffer.target = tAttachmentTarget::Depth;

	glGenTextures(1, &buffer.id);
	glBindTexture(GL_TEXTURE_2D, buffer.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer.id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
	{
		if (insertPos == -1)
			m_attachment.push_back(buffer);
		else
			m_attachment.insert(m_attachment.begin() + insertPos, buffer);
	}
}
//=============================================================================
void tFramebuffer::addColorTextureCubemapAttachment(int width, int height, int insertPos)
{
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	tAttachment buffer;
	buffer.type = AttachmentType::TextureCubeMap;
	buffer.target = tAttachmentTarget::ColorRGBA;
	GLint internalFormat = (m_hdr) ? GL_RGBA16F : GL_RGBA;
	GLenum type = (m_hdr) ? GL_FLOAT : GL_UNSIGNED_BYTE;

	glGenTextures(1, &buffer.id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, buffer.id);
	for (int i{ 0 }; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, GL_RGBA, type, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, buffer.id, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
	{
		if (insertPos == -1)
			m_attachment.push_back(buffer);
		else
			m_attachment.insert(m_attachment.begin() + insertPos, buffer);
	}
}
//=============================================================================
void tFramebuffer::addDepthTextureCubemapAttachment(int width, int height, int insertPos)
{
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	tAttachment buffer;
	buffer.type = AttachmentType::TextureCubeMap;
	buffer.target = tAttachmentTarget::Depth;

	glGenTextures(1, &buffer.id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, buffer.id);
	for (int i{ 0 }; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, buffer.id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
	{
		if (insertPos == -1)
			m_attachment.push_back(buffer);
		else
			m_attachment.insert(m_attachment.begin() + insertPos, buffer);
	}
}
//=============================================================================
void tFramebuffer::addColorRenderbufferAttachment(int width, int height, tAttachmentTarget target, int insertPos)
{
	tAttachment buffer;
	buffer.type = AttachmentType::RenderBuffer;
	buffer.target = target;
	GLint internalFormat = (m_hdr) ? GL_RGBA16F : GL_RGBA8;
	if (target == tAttachmentTarget::ColorRed)
	{
		internalFormat = GL_RED; // TODO: hdr?
	}
	else if (target == tAttachmentTarget::ColorRG)
	{
		internalFormat = (m_hdr) ? GL_RG16F : GL_RG8;
	}
	else if (target == tAttachmentTarget::ColorRGB)
	{
		internalFormat = (m_hdr) ? GL_RGB16F : GL_RGB8;
	}

	glGenRenderbuffers(1, &buffer.id);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer.id);
	if (m_multiSample)
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, internalFormat, width, height);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, buffer.id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete!");
	else
	{
		if (insertPos == -1)
			m_attachment.push_back(buffer);
		else
			m_attachment.insert(m_attachment.begin() + insertPos, buffer);
	}
}
//=============================================================================
void tFramebuffer::addDepthRenderbufferAttachment(int width, int height, int insertPos)
{
	tAttachment buffer;
	buffer.type = AttachmentType::RenderBuffer;
	buffer.target = tAttachmentTarget::Depth;

	glGenRenderbuffers(1, &buffer.id);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer.id);
	if (m_multiSample)
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, width, height);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer.id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete !");
	else
	{
		if (insertPos == -1)
			m_attachment.push_back(buffer);
		else
			m_attachment.insert(m_attachment.begin() + insertPos, buffer);
	}
}
//=============================================================================
void tFramebuffer::addDepthStencilRenderbufferAttachment(int width, int height, int insertPos)
{
	tAttachment buffer;
	buffer.type = AttachmentType::RenderBuffer;
	buffer.target = tAttachmentTarget::DepthStencil;

	glGenRenderbuffers(1, &buffer.id);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer.id);
	if (m_multiSample)
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer.id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Error("framebuffer is not complete!");
	else
	{
		if (insertPos == -1)
			m_attachment.push_back(buffer);
		else
			m_attachment.insert(m_attachment.begin() + insertPos, buffer);
	}
}
//=============================================================================
void tFramebuffer::updateColorTextureAttachment(int width, int height, tAttachmentTarget target, int insertPos)
{
	if (m_multiSample)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
	else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glDeleteTextures(1, &m_attachment.at(insertPos).id);

	m_attachment.erase(m_attachment.begin() + insertPos);

	addColorTextureAttachment(width, height, target, (insertPos == m_attachment.size() ? -1 : insertPos));
}
//=============================================================================
void tFramebuffer::updateDepthTextureAttachment(int width, int height, int insertPos)
{
	if (m_multiSample)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
	else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	glDeleteTextures(1, &m_attachment.at(insertPos).id);

	m_attachment.erase(m_attachment.begin() + insertPos);

	if (insertPos == m_attachment.size())
		addDepthTextureAttachment(width, height, -1);
	else
		addDepthTextureAttachment(width, height, insertPos);
}
//=============================================================================
void tFramebuffer::updateColorTextureCubemapAttachment(int width, int height, int insertPos)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
	glDeleteTextures(1, &m_attachment.at(insertPos).id);

	m_attachment.erase(m_attachment.begin() + insertPos);

	if (insertPos == m_attachment.size())
		addColorTextureCubemapAttachment(width, height, -1);
	else
		addColorTextureCubemapAttachment(width, height, insertPos);
}
//=============================================================================
void tFramebuffer::updateDepthTextureCubemapAttachment(int width, int height, int insertPos)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
	glDeleteTextures(1, &m_attachment.at(insertPos).id);

	m_attachment.erase(m_attachment.begin() + insertPos);

	if (insertPos == m_attachment.size())
		addDepthTextureCubemapAttachment(width, height, -1);
	else
		addDepthTextureCubemapAttachment(width, height, insertPos);
}
//=============================================================================
void tFramebuffer::updateColorRenderbufferAttachment(int width, int height, tAttachmentTarget target, int insertPos)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
	glDeleteRenderbuffers(1, &m_attachment.at(insertPos).id);

	m_attachment.erase(m_attachment.begin() + insertPos);

	addColorRenderbufferAttachment(width, height, target, (insertPos == m_attachment.size() ? -1 : insertPos));
}
//=============================================================================
void tFramebuffer::updateDepthRenderbufferAttachment(int width, int height, int insertPos)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glDeleteRenderbuffers(1, &m_attachment.at(insertPos).id);

	m_attachment.erase(m_attachment.begin() + insertPos);

	if (insertPos == m_attachment.size())
		addDepthRenderbufferAttachment(width, height, -1);
	else
		addDepthRenderbufferAttachment(width, height, insertPos);
}
//=============================================================================
void tFramebuffer::updateDepthStencilRenderbufferAttachment(int width, int height, int insertPos)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
	glDeleteRenderbuffers(1, &m_attachment.at(insertPos).id);

	m_attachment.erase(m_attachment.begin() + insertPos);

	if (insertPos == m_attachment.size())
		addDepthStencilRenderbufferAttachment(width, height, -1);
	else
		addDepthStencilRenderbufferAttachment(width, height, insertPos);
}
//=============================================================================

#pragma endregion