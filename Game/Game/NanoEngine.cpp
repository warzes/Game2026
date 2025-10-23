#include "stdafx.h"
#include "NanoEngine.h"
#include "NanoWindow.h"
#include "NanoRender.h"
#include "NanoLog.h"
//=============================================================================
#if defined(_WIN32)
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif
//=============================================================================
namespace
{
	// timing
	float       deltaTime{ 0.0f };
	double      currentTime{ 0.0 };
	double      lastFrameTime{ 0.0 };
	// fps
	const float avgInterval{ 0.5f };
	unsigned    frameCounter{ 0 };
	double      timeCounter{ 0.0 };
	float       framesPerSecond{ 0.0f };
}
//=============================================================================
#if defined(_DEBUG)
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
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "Source: Window Manager";
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
			case GL_DEBUG_SEVERITY_NOTIFICATION: return "Severity: notification";
			case GL_DEBUG_SEVERITY_LOW:          return "Severity: low";
			case GL_DEBUG_SEVERITY_MEDIUM:       return "Severity: medium";
			case GL_DEBUG_SEVERITY_HIGH:         return "Severity: high";
			}
			return "";
			}();

		const std::string msg = "OpenGL Debug message(id=" + std::to_string(id) + "):\n"
			+ sourceStr + '\n'
			+ typeStr + '\n'
			+ severityStr + '\n'
			+ "Message: " + std::string(message) + '\n';
		Error(msg);
	}
}
#endif
//=============================================================================
bool engine::Init(uint16_t width, uint16_t height, std::string_view title)
{
	if (!window::Init(width, height, title))
		return false;
	input::Init();

	// initOpenGL
	{
#if defined(_DEBUG)
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openGLErrorCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

		glEnable(GL_FRAMEBUFFER_SRGB);
		glDisable(GL_DITHER);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glViewport(0, 0, window::GetWidth(), window::GetHeight());
	}

	// initImGui
	{

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window::handle, false);
		ImGui_ImplOpenGL3_Init("#version 330");
		ImGui::StyleColorsDark();

		GLFWmonitor* primary = glfwGetPrimaryMonitor();

		float xscale, yscale;
		glfwGetMonitorContentScale(primary, &xscale, &yscale);

		ImGuiStyle* style = &ImGui::GetStyle();
		style->ScaleAllSizes(xscale > yscale ? xscale : yscale);

		ImGuiIO& io = ImGui::GetIO();
		io.FontGlobalScale = xscale > yscale ? xscale : yscale;
		io.IniFilename = nullptr;

		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;// TODO: возможно есть другой способ как имгуи не давать показывать скрытый курсор
	}

	if (!textures::Init())
		return false;

	deltaTime = 0.0f;
	lastFrameTime = glfwGetTime();

	return true;
}
//=============================================================================
void engine::Close() noexcept
{
	textures::Close();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	window::Close();
}
//=============================================================================
bool engine::ShouldClose()
{
	return window::WindowShouldClose();
}
//=============================================================================
void engine::BeginFrame()
{
	// calc deltaTime
	{
		currentTime = glfwGetTime();
		deltaTime = static_cast<float>(currentTime - lastFrameTime);
		lastFrameTime = currentTime;
	}

	// calc fps
	{
		frameCounter++;
		timeCounter += deltaTime;
		if (timeCounter > avgInterval)
		{
			framesPerSecond = static_cast<float>(frameCounter) / avgInterval;
			frameCounter = 0;
			timeCounter = 0.0;
		}
	}

	// Start a new ImGUi frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}
//=============================================================================
void engine::EndFrame()
{
	// Updates ImGui
	ImGui::Render();
	auto* drawData = ImGui::GetDrawData();
	if (drawData->CmdListsCount > 0)
	{
		glDisable(GL_FRAMEBUFFER_SRGB);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		ImGui_ImplOpenGL3_RenderDrawData(drawData);
		glEnable(GL_FRAMEBUFFER_SRGB);
	}

	window::Swap();
	input::Update();
}
//=============================================================================
void engine::DrawFPS()
{
	if (const ImGuiViewport* v = ImGui::GetMainViewport())
	{
		ImGui::SetNextWindowPos({ v->WorkPos.x + v->WorkSize.x - 15.0f, v->WorkPos.y + 15.0f }, ImGuiCond_Always, { 1.0f, 0.0f });
	}
	ImGui::SetNextWindowBgAlpha(0.30f);
	ImGui::SetNextWindowSize(ImVec2(ImGui::CalcTextSize("FPS : _______").x, 0));
	if (ImGui::Begin("##FPS", nullptr,
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove))
	{
		ImGui::Text("FPS : %i", (int)framesPerSecond);
		ImGui::Text("Ms  : %.1f", framesPerSecond > 0 ? 1000.0 / framesPerSecond : 0);
	}
	ImGui::End();
}
//=============================================================================
float engine::GetDeltaTime()
{
	return deltaTime;
}
//=============================================================================