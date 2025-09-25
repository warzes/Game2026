#pragma once

namespace engine
{
	bool Init(uint16_t width, uint16_t height, std::string_view title);
	void Close() noexcept;

	bool ShouldClose();
	void BeginFrame();
	void EndFrame();
}