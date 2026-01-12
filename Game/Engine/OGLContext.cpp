#include "stdafx.h"
#include "OGLContext.h"
#include "NanoLog.h"
#include "NanoOpenGL3.h"
//=============================================================================
#if defined(_WIN32)
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif
//=============================================================================
extern std::unordered_map<SamplerStateInfo, SamplerHandle> SamplerCache;
//#if defined(_DEBUG)
namespace
{
	void openGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]] GLsizei length, const GLchar* message, [[maybe_unused]] const void* user_param) noexcept
	{
		// Ignore certain verbose info messages (particularly ones on Nvidia).
		if (id == 131169 ||
			id == 131185 || // NV: Buffer will use video memory
			id == 131218 ||
			id == 131204 || // Texture cannot be used for texture mapping
			id == 131222 ||
			id == 131154 || // NV: pixel transfer is synchronized with 3D rendering
			id == 0         // gl{Push, Pop}DebugGroup
			)
			return;

		const auto sourceStr = [source]() {
			switch (source)
			{
			case GL_DEBUG_SOURCE_API:             return "Source: API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "Source: Window System";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Source: Shader Compiler";
			case GL_DEBUG_SOURCE_THIRD_PARTY:     return "Source: Third Party";
			case GL_DEBUG_SOURCE_APPLICATION:     return "Source: Application";
			case GL_DEBUG_SOURCE_OTHER:           return "Source: Other";
			}
			return "";
			}();

		const auto typeStr = [type]() {
			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR:               return "Type: Error";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Type: Deprecated Behavior";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Type: Undefined Behavior";
			case GL_DEBUG_TYPE_PORTABILITY:         return "Type: Portability";
			case GL_DEBUG_TYPE_PERFORMANCE:         return "Type: Performance";
			case GL_DEBUG_TYPE_MARKER:              return "Type: Marker";
			case GL_DEBUG_TYPE_PUSH_GROUP:          return "Type: Push Group";
			case GL_DEBUG_TYPE_POP_GROUP:           return "Type: Pop Group";
			case GL_DEBUG_TYPE_OTHER:               return "Type: Other";
			}
			return "";
			}();

		const auto severityStr = [severity]() {
			switch (severity) {
			case GL_DEBUG_SEVERITY_NOTIFICATION: return "Severity: Notification";
			case GL_DEBUG_SEVERITY_LOW:          return "Severity: Low";
			case GL_DEBUG_SEVERITY_MEDIUM:       return "Severity: Medium";
			case GL_DEBUG_SEVERITY_HIGH:         return "Severity: High";
			}
			return "";
			}();

		const std::string msg = "OpenGL Debug Message:(id=" + std::to_string(id) + "):\n"
			+ sourceStr + '\n'
			+ typeStr + '\n'
			+ severityStr + '\n'
			+ "Message: " + std::string(message) + '\n';
		Error(msg);
	}
}
//#endif
//=============================================================================
bool OGLContextInit()
{
	// glad: load all OpenGL function pointers
	const int openGLVersion = gladLoadGL(RGFW_getProcAddress_OpenGL);
	if (openGLVersion < GLAD_MAKE_VERSION(3, 3))
	{
		Fatal("Failed to initialize OpenGL context!");
		return false;
	}

	const char* renderer = (const char*)glGetString(GL_RENDERER);
	const char* version = (const char*)glGetString(GL_VERSION);
	Print("Renderer: " + std::string(renderer));
	Print("OpenGL version supported " + std::string(version));

	// enable debug context
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		Print("Enable OpenGL Debug Context");
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
		glDebugMessageCallback(openGLErrorCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);

	// TODO: reset opengl state

	return true;
}
//=============================================================================
void OGLContextClose()
{
	for (const auto& [_, sampler] : SamplerCache)
	{
		glDeleteSamplers(1, &sampler.handle);
	}
	SamplerCache.clear();
}
//=============================================================================
void OGLContext::SetClearColor(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
}
//=============================================================================
void OGLContext::Clear(bool colorBuffer, bool depthBuffer, bool stencilBuffer)
{
	GLbitfield clearMask = 0;
	if (colorBuffer) clearMask |= GL_COLOR_BUFFER_BIT;
	if (depthBuffer) clearMask |= GL_DEPTH_BUFFER_BIT;
	if (stencilBuffer) clearMask |= GL_STENCIL_BUFFER_BIT;

	if (clearMask != 0)
	{
		glClear(clearMask);
	}
}
//=============================================================================
void OGLContext::SetRasterizationLinesWidth(float width)
{
	glLineWidth(width);
}
//=============================================================================
void OGLContext::SetRasterizationMode(RasterizationMode rasterizationMode)
{
	glPolygonMode(GL_FRONT_AND_BACK, EnumToValue(rasterizationMode));
}
//=============================================================================
void OGLContext::DrawElements(PrimitiveMode primitiveMode, uint32_t indexCount)
{
	glDrawElements(EnumToValue(primitiveMode), indexCount, GL_UNSIGNED_INT, nullptr);
}
//=============================================================================
void OGLContext::DrawElementsInstanced(PrimitiveMode primitiveMode, uint32_t indexCount, uint32_t instances)
{
	glDrawElementsInstanced(EnumToValue(primitiveMode), indexCount, GL_UNSIGNED_INT, nullptr, instances);
}
//=============================================================================
void OGLContext::DrawArrays(PrimitiveMode primitiveMode, uint32_t vertexCount)
{
	glDrawArrays(EnumToValue(primitiveMode), 0, vertexCount);
}
//=============================================================================
void OGLContext::DrawArraysInstanced(PrimitiveMode primitiveMode, uint32_t vertexCount, uint32_t instances)
{
	glDrawArraysInstanced(EnumToValue(primitiveMode), 0, vertexCount, instances);
}
//=============================================================================