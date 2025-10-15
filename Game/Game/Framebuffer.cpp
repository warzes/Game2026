#include "stdafx.h"
#include "Framebuffer.h"
#include "NanoLog.h"
// TODO: отрефакторить
// после создания текстуры вернуть бинд старой
//=============================================================================
Framebuffer::Framebuffer(bool color, bool ms, bool hdr)
	: m_renderColor(color)
	, m_multiSample(ms)
	, m_hdr(hdr)
{
	glGenFramebuffers(1, &m_fbo);
}
//=============================================================================
Framebuffer::~Framebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	for (int i{ 0 }; i < m_attachment.size(); ++i)
	{
		if (m_attachment.at(i).type == AttachmentType::Texture)
		{
			switch (m_attachment.at(i).target)
			{
			case AttachmentTarget::Color:
				if (m_multiSample)
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
				else
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
				break;
			case AttachmentTarget::Depth:
				if (m_multiSample)
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
				else
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
				break;
			case AttachmentTarget::Stencil:
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
			case AttachmentTarget::Color:
				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
				break;
			case AttachmentTarget::Depth:
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
			case AttachmentTarget::Color:
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
				break;
			case AttachmentTarget::Depth:
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
				break;
			case AttachmentTarget::Stencil:
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
				break;
			case AttachmentTarget::DepthStencil:
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
void Framebuffer::AddAttachment(AttachmentType type, AttachmentTarget target, int width, int height, int insertPos)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	if (type == AttachmentType::Texture)
	{
		switch (target)
		{
		case AttachmentTarget::Color: addColorTextureAttachment(width, height, insertPos); break;
		case AttachmentTarget::Depth: addDepthTextureAttachment(width, height, insertPos); break;
		default: break;
		}
	}
	else if (type == AttachmentType::TextureCubeMap)
	{
		switch (target)
		{
		case AttachmentTarget::Color: addColorTextureCubemapAttachment(width, height, insertPos); break;
		case AttachmentTarget::Depth: addDepthTextureCubemapAttachment(width, height, insertPos); break;
		default: break;
		}
	}
	else if (type == AttachmentType::RenderBuffer)
	{
		switch (target)
		{
		case AttachmentTarget::Color: addColorRenderbufferAttachment(width, height, insertPos); break;
		case AttachmentTarget::Depth: addDepthRenderbufferAttachment(width, height, insertPos); break;
		case AttachmentTarget::DepthStencil: addDepthStencilRenderbufferAttachment(width, height, insertPos); break;
		default: break;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::UpdateAttachment(AttachmentType type, AttachmentTarget target, int width, int height)
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
				case AttachmentTarget::Color: updateColorTextureAttachment(width, height, i); break;
				case AttachmentTarget::Depth: updateDepthTextureAttachment(width, height, i); break;
				default: break;
				}
			}
			else if (type == AttachmentType::TextureCubeMap)
			{
				switch (target)
				{
				case AttachmentTarget::Color: updateColorTextureCubemapAttachment(width, height, i); break;
				case AttachmentTarget::Depth: updateDepthTextureCubemapAttachment(width, height, i); break;
				default: break;
				}
			}
			else if (type == AttachmentType::RenderBuffer)
			{
				switch (target)
				{
				case AttachmentTarget::Color: updateColorRenderbufferAttachment(width, height, i); break;
				case AttachmentTarget::Depth: updateDepthRenderbufferAttachment(width, height, i); break;
				case AttachmentTarget::DepthStencil: updateDepthStencilRenderbufferAttachment(width, height, i); break;
				default: break;
				}
			}
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::CreateDirectionalDepthFBO(int width, int height)
{
	m_multiSample = false;
	m_renderColor = false;
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Attachment buffer;
	buffer.type = AttachmentType::Texture;
	buffer.target = AttachmentTarget::Depth;

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
void Framebuffer::CreateOmnidirectionalDepthFBO(int width, int height)
{
	m_multiSample = false;
	m_renderColor = false;
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Attachment buffer;
	buffer.type = AttachmentType::TextureCubeMap;
	buffer.target = AttachmentTarget::Depth;

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
void Framebuffer::CreateMultisampledFBO(int width, int height)
{
	m_multiSample = true;
	m_renderColor = true;
	struct Attachment bufferColor;
	bufferColor.type = AttachmentType::Texture;
	bufferColor.target = AttachmentTarget::Color;

	Attachment bufferDS;
	bufferDS.type = AttachmentType::RenderBuffer;
	bufferDS.target = AttachmentTarget::DepthStencil;

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
void Framebuffer::CreateResolveFBO(int width, int height)
{
	m_multiSample = false;
	m_renderColor = true;
	
	Attachment buffer;
	buffer.type = AttachmentType::Texture;
	buffer.target = AttachmentTarget::Color;

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
void Framebuffer::UpdateDirectionalDepthFBO(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	glDeleteTextures(1, &m_attachment.at(0).id);
	m_attachment.clear();
	CreateDirectionalDepthFBO(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::UpdateOmnidirectionalDepthFBO(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
	glDeleteTextures(1, &m_attachment.at(0).id);
	m_attachment.clear();
	CreateOmnidirectionalDepthFBO(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::UpdateMultisampledFBO(int width, int height)
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
void Framebuffer::UpdateResolveFBO(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glDeleteTextures(1, &m_attachment.at(0).id);
	m_attachment.clear();
	CreateResolveFBO(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
std::vector<Attachment>& Framebuffer::GetAttachments()
{
	return m_attachment;
}
//=============================================================================
void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}
//=============================================================================
void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::BlitFramebuffer(Framebuffer& writeFBO, int width, int height)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, writeFBO.GetId());
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
//=============================================================================
void Framebuffer::BlitFramebuffer(std::unique_ptr<Framebuffer>& writeFBO, int width, int height)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, writeFBO->GetId());
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
//=============================================================================
void Framebuffer::addColorTextureAttachment(int width, int height, int insertPos)
{
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Attachment buffer;
	buffer.type = AttachmentType::Texture;
	buffer.target = AttachmentTarget::Color;
	GLint internalFormat = (m_hdr) ? GL_RGBA16F : GL_RGBA;
	GLenum type = (m_hdr) ? GL_FLOAT : GL_UNSIGNED_BYTE;

	if (m_multiSample)
	{
		glGenTextures(1, &buffer.id);
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
		glBindTexture(GL_TEXTURE_2D, buffer.id);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, type, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.id, 0);
	}

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
void Framebuffer::addDepthTextureAttachment(int width, int height, int insertPos)
{
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Attachment buffer;
	buffer.type = AttachmentType::Texture;
	buffer.target = AttachmentTarget::Depth;

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
void Framebuffer::addColorTextureCubemapAttachment(int width, int height, int insertPos)
{
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Attachment buffer;
	buffer.type = AttachmentType::TextureCubeMap;
	buffer.target = AttachmentTarget::Color;
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
void Framebuffer::addDepthTextureCubemapAttachment(int width, int height, int insertPos)
{
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Attachment buffer;
	buffer.type = AttachmentType::TextureCubeMap;
	buffer.target = AttachmentTarget::Depth;

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
void Framebuffer::addColorRenderbufferAttachment(int width, int height, int insertPos)
{
	Attachment buffer;
	buffer.type = AttachmentType::RenderBuffer;
	buffer.target = AttachmentTarget::Color;
	GLint internalFormat = (m_hdr) ? GL_RGBA16F : GL_RGBA8;

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
void Framebuffer::addDepthRenderbufferAttachment(int width, int height, int insertPos)
{
	Attachment buffer;
	buffer.type = AttachmentType::RenderBuffer;
	buffer.target = AttachmentTarget::Depth;

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
void Framebuffer::addDepthStencilRenderbufferAttachment(int width, int height, int insertPos)
{
	Attachment buffer;
	buffer.type = AttachmentType::RenderBuffer;
	buffer.target = AttachmentTarget::DepthStencil;

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
void Framebuffer::updateColorTextureAttachment(int width, int height, int insertPos)
{
	if (m_multiSample)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
	else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glDeleteTextures(1, &m_attachment.at(insertPos).id);

	m_attachment.erase(m_attachment.begin() + insertPos);

	if (insertPos == m_attachment.size())
		addColorTextureAttachment(width, height, -1);
	else
		addColorTextureAttachment(width, height, insertPos);
}
//=============================================================================
void Framebuffer::updateDepthTextureAttachment(int width, int height, int insertPos)
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
void Framebuffer::updateColorTextureCubemapAttachment(int width, int height, int insertPos)
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
void Framebuffer::updateDepthTextureCubemapAttachment(int width, int height, int insertPos)
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
void Framebuffer::updateColorRenderbufferAttachment(int width, int height, int insertPos)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
	glDeleteRenderbuffers(1, &m_attachment.at(insertPos).id);

	m_attachment.erase(m_attachment.begin() + insertPos);

	if (insertPos == m_attachment.size())
		addColorRenderbufferAttachment(width, height, -1);
	else
		addColorRenderbufferAttachment(width, height, insertPos);
}
//=============================================================================
void Framebuffer::updateDepthRenderbufferAttachment(int width, int height, int insertPos)
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
void Framebuffer::updateDepthStencilRenderbufferAttachment(int width, int height, int insertPos)
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