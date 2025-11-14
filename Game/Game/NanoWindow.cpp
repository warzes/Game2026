#include "stdafx.h"
#include "NanoWindow.h"
#include "NanoLog.h"
//=============================================================================
namespace
{
	bool windowQuit{ true };
	RGFW_event windowEvent;
	uint16_t windowWidth, windowHeight;
	float windowAspect{ 1.0f };

	glm::vec2 cursorPos{};
	glm::vec2 cursorOffset{};
	glm::vec2 scrollOffset{};
}
//=============================================================================
void errorFunc(RGFW_debugType type, RGFW_errorCode err, const char* msg) noexcept
{
	if (type != RGFW_typeError || err == RGFW_noError) return; /* disregard non-errors */
	Error("RGFW ERROR: " + std::string(msg));
}
//=============================================================================
void windowResizeFunc(RGFW_window* win, i32 width, i32 height) noexcept
{
	if (width < 0 || height < 0) return;
	windowWidth = static_cast<uint16_t>(std::max(width, 1));
	windowHeight = static_cast<uint16_t>(std::max(height, 1));
	windowAspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
}
//=============================================================================
void windowMinimizeFunc(RGFW_window*) noexcept
{
	Debug("window minimize");
}
//=============================================================================
void windowMaximizeFunc(RGFW_window*, i32 x, i32 y, i32 w, i32 h) noexcept
{
	RGFW_UNUSED(x); RGFW_UNUSED(y);
	Debug("window maximize: " + std::to_string(w) + ":" + std::to_string(h));
}
//=============================================================================
void windowRestoreFunc(RGFW_window*, i32 x, i32 y, i32 w, i32 h) noexcept
{
	RGFW_UNUSED(x); RGFW_UNUSED(y);
	Debug("window restore: " + std::to_string(w) + ":" + std::to_string(h));
}
//=============================================================================
void windowQuitFunc(RGFW_window*) noexcept
{
}
//=============================================================================
void focusFunc(RGFW_window* win, u8 inFocus) noexcept
{
	ImGui_ImplRgfw_WindowFocusCallback(win, inFocus);
	if (inFocus) Debug("window in focus");
	else Debug("window out of focus");
}
//=============================================================================
void mouseNotifyFunc(RGFW_window* win, i32 x, i32 y, u8 status) noexcept
{
	ImGui_ImplRgfw_CursorEnterCallback(win, x, y, status);

	if (status) Debug("mouse enter " + std::to_string(x) + ":" + std::to_string(y));
	else Debug("mouse leave");
}
//=============================================================================
void mousePosFunc(RGFW_window* window, i32 xPos, i32 yPos, float vecX, float vecY) noexcept
{
	ImGui_ImplRgfw_CursorPosCallback(window, xPos, yPos, vecX, vecY);
	cursorPos.x = static_cast<float>(xPos);
	cursorPos.y = static_cast<float>(yPos);
	cursorOffset.x = vecX;
	cursorOffset.y = -vecY; // Reversed Y since y-coordinates go from bottom to left
}
//=============================================================================
void mouseButtonFunc(RGFW_window* win, u8 button, u8 pressed) noexcept
{
	ImGui_ImplRgfw_MouseButtonCallback(win, button, pressed);
}
//=============================================================================
void scrollFunc(RGFW_window* win, float xOffset, float yOffset) noexcept
{
	ImGui_ImplRgfw_MouseScrollCallback(win, xOffset, yOffset);
	scrollOffset.x = xOffset;
	scrollOffset.y = yOffset;
}
//=============================================================================
void keyFunc(RGFW_window* win, RGFW_key key, u8 keyChar, RGFW_keymod keyMod, RGFW_bool repeat, RGFW_bool pressed) noexcept
{
	ImGui_ImplRgfw_KeyCallback(win, key, keyChar, keyMod, repeat, pressed);
}
//=============================================================================
bool window::Init(uint16_t width, uint16_t height, std::string_view title, bool vsync, bool resizable, bool maximized)
{
	if (!RGFW_init())
	{
		Fatal("Error Initialising RGFW");
		return false;
	}
	RGFW_glHints* hints = RGFW_getGlobalHints_OpenGL();
	hints->depth = 32;
	hints->sRGB = true;
#if defined(_DEBUG)
	hints->debug = true;
#else
	hints->noError = true;
#endif
	hints->major = 3;
	hints->minor = 3;

	RGFW_setGlobalHints_OpenGL(hints);

	RGFW_windowFlags windowFlags = RGFW_windowCenter | RGFW_windowOpenGL;
	if (!resizable) windowFlags |= RGFW_windowNoResize;
	if (maximized) windowFlags |= RGFW_windowMaximize;

	handle = RGFW_createWindow(title.data(), 0, 0, width,height, windowFlags);
	if (!handle)
	{
		Fatal("Failed to create RGFW window");
		return false;
	}

	// Window callbacks
	RGFW_setDebugCallback(errorFunc);
	RGFW_setWindowResizedCallback(windowResizeFunc);
	RGFW_setWindowMinimizedCallback(windowMinimizeFunc);
	RGFW_setWindowRestoredCallback(windowRestoreFunc);
	RGFW_setWindowMaximizedCallback(windowMaximizeFunc);
	RGFW_setWindowQuitCallback(windowQuitFunc);
	RGFW_setFocusCallback(focusFunc);
	//// Mouse callbacks
	RGFW_setMouseNotifyCallback(mouseNotifyFunc);
	RGFW_setMousePosCallback(mousePosFunc);
	RGFW_setMouseButtonCallback(mouseButtonFunc);
	RGFW_setMouseScrollCallback(scrollFunc);
	//// Key callbacks
	RGFW_setKeyCallback(keyFunc);


	// Get buffer size information
	int displayW, displayH;
	RGFW_window_getSize(handle, &displayW, &displayH);
	windowWidth = static_cast<uint16_t>(displayW);
	windowHeight = static_cast<uint16_t>(displayH);
	windowAspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

	// Set the current context
	RGFW_window_makeCurrentContext_OpenGL(handle);
		
	// glad: load all OpenGL function pointers
	const int openGLVersion = gladLoadGL(RGFW_getProcAddress_OpenGL);
	if (openGLVersion < GLAD_MAKE_VERSION(3, 3))
	{
		Fatal("Failed to initialize OpenGL context!");
		return false;
	}

	RGFW_window_swapInterval_OpenGL(handle, vsync ? 1 : 0);

	windowQuit = false;

	return true;
}
//=============================================================================
void window::Close() noexcept
{
	windowQuit = true;
	if (handle) RGFW_window_close(handle);
	handle = nullptr;
	RGFW_deinit();
}
//=============================================================================
bool window::WindowShouldClose() noexcept
{
	bool eee = RGFW_window_shouldClose(handle) == RGFW_TRUE || windowQuit;
	if (eee)
	{
		windowQuit = eee;
	}

	return RGFW_window_shouldClose(handle) == RGFW_TRUE || windowQuit;
}
//=============================================================================
void window::Swap()
{
	RGFW_window_swapBuffers_OpenGL(handle);
}
//=============================================================================
uint16_t window::GetWidth() noexcept { return windowWidth; }
uint16_t window::GetHeight() noexcept { return windowHeight; }
float window::GetAspect() noexcept { return windowAspect; }
//=============================================================================
void input::Init()
{
	i32 xpos, ypos;
	RGFW_window_getMouse(window::handle, &xpos, &ypos);
	cursorOffset.x = static_cast<float>(xpos);
	cursorOffset.y = static_cast<float>(ypos);
}
//=============================================================================
void input::Update()
{
	scrollOffset = glm::vec2(0);
	cursorOffset = glm::vec2(0);

	while (RGFW_window_checkEvent(window::handle, &windowEvent))
	{
		if (windowEvent.type == RGFW_quit)
		{
			windowQuit = true;
		}
		// TODO: ???
	}
}
//=============================================================================
void input::SetCursorVisible(bool state)
{
	if (state)
	{
		if (RGFW_window_isHoldingMouse(window::handle))
		{
			RGFW_window_showMouse(window::handle, 1);
			RGFW_window_unholdMouse(window::handle);
		}
	}
	else
	{
		if (!RGFW_window_isHoldingMouse(window::handle))
		{
			RGFW_window_showMouse(window::handle, 0);
			RGFW_window_holdMouse(window::handle);
		}
	}
}
//=============================================================================
const glm::vec2& input::GetCursorPos() noexcept { return cursorPos; }
const glm::vec2& input::GetCursorOffset() noexcept { return cursorOffset; }
const glm::vec2& input::GetScrollOffset() noexcept { return scrollOffset; }
//=============================================================================
bool input::IsKeyDown(RGFW_key key) noexcept { return RGFW_isKeyDown(key); }
bool input::IsKeyUp(RGFW_key key) noexcept { return !RGFW_isKeyDown(key); }
bool input::IsKeyPressed(RGFW_key key) noexcept { return RGFW_isKeyPressed(key); }
bool input::IsKeyReleased(RGFW_key key) noexcept { return RGFW_isKeyReleased(key); }
bool input::IsMouseDown(RGFW_mouseButton key) noexcept { return RGFW_isMouseDown(key); }
bool input::IsMouseUp(RGFW_mouseButton key) noexcept { return !RGFW_isMouseDown(key); }
bool input::IsMousePressed(RGFW_mouseButton key) noexcept { return RGFW_isMousePressed(key); }
bool input::IsMouseReleased(RGFW_mouseButton key) noexcept { return RGFW_isMouseReleased(key); }
//=============================================================================