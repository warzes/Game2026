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
	std::chrono::high_resolution_clock::time_point previousTime;
	std::chrono::high_resolution_clock::time_point currentTime;


	// fps
	const float avgInterval{ 0.5f };
	unsigned    frameCounter{ 0 };
	double      timeCounter{ 0.0 };
	float       framesPerSecond{ 0.0f };
}
//=============================================================================
bool engine::Init(uint16_t width, uint16_t height, std::string_view title)
{
	if (!window::Init(width, height, title))
		return false;
	input::Init();

	if (!oglSystem::Init())
		return false;

	EnableSRGB(true);
	glViewport(0, 0, window::GetWidth(), window::GetHeight());

	// initImGui
	{

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplRgfw_InitForOpenGL(window::handle, false);
		ImGui_ImplOpenGL3_Init("#version 330");
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;// TODO: возможно есть другой способ как имгуи не давать показывать скрытый курсор
	}

	if (!textures::Init())
		return false;

	deltaTime = 0.0f;
	previousTime = std::chrono::high_resolution_clock::now();

	return true;
}
//=============================================================================
void engine::Close() noexcept
{
	textures::Close();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplRgfw_Shutdown();
	ImGui::DestroyContext();
	oglSystem::Close();
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
		currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(currentTime - previousTime).count();
		previousTime = currentTime;
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
	ImGui_ImplRgfw_NewFrame();
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
		EnableSRGB(false);
		ImGui_ImplOpenGL3_RenderDrawData(drawData);
		EnableSRGB(true);
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