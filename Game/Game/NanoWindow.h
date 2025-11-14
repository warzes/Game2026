#pragma once

namespace window
{
	bool Init(uint16_t width, uint16_t height, std::string_view title, bool vsync = false, bool resizable = true, bool maximized = false);
	void Close() noexcept;

	bool WindowShouldClose() noexcept;

	void Swap();

	uint16_t GetWidth() noexcept;
	uint16_t GetHeight() noexcept;
	float    GetAspect() noexcept;

	inline RGFW_window* handle{ nullptr };

} // namespace window

namespace input
{
	void Init();
	void Update();

	const glm::vec2& GetCursorPos() noexcept;
	const glm::vec2& GetCursorOffset() noexcept;
	const glm::vec2& GetScrollOffset() noexcept;

	bool IsKeyDown(RGFW_key key) noexcept;
	bool IsKeyUp(RGFW_key key) noexcept;
	bool IsKeyPressed(RGFW_key key) noexcept;
	bool IsKeyReleased(RGFW_key key) noexcept;
	bool IsMouseDown(RGFW_mouseButton key) noexcept;
	bool IsMouseUp(RGFW_mouseButton key) noexcept;
	bool IsMousePressed(RGFW_mouseButton key) noexcept;
	bool IsMouseReleased(RGFW_mouseButton key) noexcept;

	void SetCursorVisible(bool state);
} // namespace input