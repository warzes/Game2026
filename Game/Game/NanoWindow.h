#pragma once

enum CursorState : uint8_t
{
	Normal,
	Hidden,
	Disabled
};

enum KeyState : uint8_t
{
	Down,
	Pressed,
	Up,
	Released,
	Repeat
};

constexpr inline size_t MaxKeys{ GLFW_KEY_LAST };
constexpr inline size_t MaxMouseButtons{ GLFW_MOUSE_BUTTON_LAST };

// keycodes can be negative in case of an error
using Key = int;
using MouseButton = int;

struct InputKey { Key key{}; };
struct InputMouseButton { MouseButton button; };
struct InputMouseScroll { bool yaxis{ false }; };
struct InputMousePos { bool yaxis{ false }; };

namespace window
{
	bool Init(uint16_t width, uint16_t height, std::string_view title, bool vsync = false, bool resizable = true, bool maximized = false, bool decorate = false);
	void Close() noexcept;

	bool WindowShouldClose() noexcept;

	void Swap();

	uint16_t GetWidth();
	uint16_t GetHeight();
	float    GetAspect();

	bool* GetsKeys(); // OLD
	float GetXChange();// OLD
	float GetYChange();// OLD

	inline GLFWwindow* handle{ nullptr };

} // namespace window

namespace input
{
	void Init();
	void Update();

	const glm::vec2& GetCursorPos();
	const glm::vec2& GetCursorOffset();
	const glm::vec2& GetPrevCursorPos();
	const glm::vec2& GetScrollOffset();

	bool IsKeyDown(Key key);
	bool IsKeyUp(Key key);
	bool IsKeyPressed(Key key);
	bool IsKeyReleased(Key key);
	bool IsMouseDown(MouseButton key);
	bool IsMouseUp(MouseButton key);
	bool IsMousePressed(MouseButton key);
	bool IsMouseReleased(MouseButton key);

	void SetCursorVisible(bool state);

	inline float sensitivity{ 0.35f };
} // namespace input