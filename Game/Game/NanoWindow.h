#pragma once

enum CursorState : uint8_t
{
	Normal,
	Hidden,
	Disabled
};

enum KeyState : uint8_t // applicable to keyboard keys and mouse buttons
{
	Down = 0b00001,
	Pressed = 0b00011,
	Up = 0b00100,
	Released = 0b01100,
	Repeat = 0b10001
};

constexpr inline size_t MaxKeys{ GLFW_KEY_LAST };
constexpr inline size_t MaxMouseButtons{ GLFW_MOUSE_BUTTON_LAST };

// keycodes can be negative in case of an error
using Key = int;
using MouseButton = int;

namespace window
{
	bool Init(uint16_t width, uint16_t height, std::string_view title, bool vsync = false, bool resizable = true, bool maximized = false, bool decorate = true);
	void Close() noexcept;

	bool WindowShouldClose() noexcept;

	void Swap();

	uint16_t GetWidth() noexcept;
	uint16_t GetHeight() noexcept;
	float    GetAspect() noexcept;

	inline GLFWwindow* handle{ nullptr };

} // namespace window

namespace input
{
	void Init();
	void Update();

	const glm::vec2& GetCursorPos() noexcept;
	const glm::vec2& GetCursorOffset() noexcept;
	const glm::vec2& GetPrevCursorPos() noexcept;
	const glm::vec2& GetScrollOffset() noexcept;

	bool IsKeyDown(Key key) noexcept;
	bool IsKeyUp(Key key) noexcept;
	bool IsKeyPressed(Key key) noexcept;
	bool IsKeyReleased(Key key) noexcept;
	bool IsMouseDown(MouseButton key) noexcept;
	bool IsMouseUp(MouseButton key) noexcept;
	bool IsMousePressed(MouseButton key) noexcept;
	bool IsMouseReleased(MouseButton key) noexcept;

	void SetCursorVisible(bool state);
} // namespace input