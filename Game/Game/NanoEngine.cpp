#include "stdafx.h"
#include "NanoEngine.h"
#include "NanoWindow.h"
//=============================================================================
namespace
{
	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
}
//=============================================================================
bool engine::Init(uint16_t width, uint16_t height, std::string_view title)
{
	if (!window::Init(width, height, title))
		return false;

	deltaTime = 0.0f;
	lastFrame = 0.0f;

	return true;
}
//=============================================================================
void engine::Close() noexcept
{
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
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}
//=============================================================================
void engine::EndFrame()
{
	window::Swap();
}
//=============================================================================