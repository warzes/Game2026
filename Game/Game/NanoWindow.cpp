#include "stdafx.h"
#include "NanoWindow.h"
#include "NanoLog.h"
//=============================================================================
namespace
{
	uint16_t windowWidth, windowHeight;
	float windowAspect{ 1.0f };

	glm::vec2 cursorPos{};
	glm::vec2 cursorOffset{};
	glm::vec2 cursorPosLastFrame{};
	glm::vec2 scrollOffset{};

	KeyState keysStatus[MaxKeys] = { KeyState(0) };
	KeyState mouseButtonStatus[MaxMouseButtons] = { KeyState(0) };
}
//=============================================================================
void keyPress(int key, int action) noexcept
{
	if (key != GLFW_KEY_UNKNOWN)
	{
		switch (action)
		{
		case GLFW_RELEASE: keysStatus[key] = KeyState::Released; break;
		case GLFW_PRESS:   keysStatus[key] = KeyState::Pressed; break;
		case GLFW_REPEAT:  keysStatus[key] = KeyState::Repeat; break;
		default:           std::unreachable(); break;
		}
	}
}
//=============================================================================
void mouseButton(int button, int action) noexcept
{
	switch (action)
	{
	case GLFW_RELEASE: mouseButtonStatus[button] = KeyState::Released; break;
	case GLFW_PRESS:   mouseButtonStatus[button] = KeyState::Pressed; break;
	case GLFW_REPEAT:  mouseButtonStatus[button] = KeyState::Repeat; break;
	default:           std::unreachable(); break;
	}
}
//=============================================================================
void mousePos(double xPos, double yPos) noexcept
{
	cursorPos.x = static_cast<float>(xPos);
	cursorPos.y = static_cast<float>(yPos);

	cursorOffset.x += cursorPos.x - cursorPosLastFrame.x;
	cursorOffset.y += cursorPosLastFrame.y - cursorPos.y; // Reversed Y since y-coordinates go from bottom to left
	cursorPosLastFrame = cursorPos;
}
//=============================================================================
void mouseScroll(double xOffset, double yOffset) noexcept
{
	scrollOffset.x = static_cast<float>(xOffset);
	scrollOffset.y = static_cast<float>(yOffset);
}
//=============================================================================
void framebufferSizeCallback([[maybe_unused]] GLFWwindow* window, int width, int height) noexcept
{
	if (width < 0 || height < 0) return;
	windowWidth = static_cast<uint16_t>(std::max(width, 1));
	windowHeight = static_cast<uint16_t>(std::max(height, 1));
	windowAspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
}
//=============================================================================
void windowFocusCallback(GLFWwindow* window, int focused) noexcept
{
	ImGui_ImplGlfw_WindowFocusCallback(window, focused);
}
//=============================================================================
void monitorCallback(GLFWmonitor* monitor, int event) noexcept
{
	ImGui_ImplGlfw_MonitorCallback(monitor, event);
}
//=============================================================================
void handleKeysCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) noexcept
{
	ImGui_ImplGlfw_KeyCallback(window, key, scanCode, action, mods);
	keyPress(key, action);
}
//=============================================================================
void handleCharCallback(GLFWwindow* window, unsigned int c) noexcept
{
	ImGui_ImplGlfw_CharCallback(window, c);
}
//=============================================================================
void handleCursorEnterCallback(GLFWwindow* window, int entered) noexcept
{
	ImGui_ImplGlfw_CursorEnterCallback(window, entered);
}
//=============================================================================
void handleMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) noexcept
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
	mouseButton(button, action);
}
//=============================================================================
void handleMouseCursorPosCallback(GLFWwindow* window, double xPos, double yPos) noexcept
{
	ImGui_ImplGlfw_CursorPosCallback(window, xPos, yPos);
	mousePos(xPos, yPos);
}
//=============================================================================
void handleMouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) noexcept
{
	ImGui_ImplGlfw_ScrollCallback(window, xOffset, yOffset);
	mouseScroll(xOffset, yOffset);
}
//=============================================================================
bool window::Init(uint16_t width, uint16_t height, std::string_view title, bool vsync, bool resizable, bool maximized, bool decorate)
{
	if (!glfwInit())
	{
		Fatal("Error Initialising GLFW");
		return false;
	}
	// Setup GLFW Windows Properties
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatiblity
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#if defined(_DEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
	glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_MAXIMIZED, maximized ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_DECORATED, decorate ? GL_TRUE : GL_FALSE);

	// Disable GlFW auto iconify behaviour
	// Auto Iconify automatically minimizes (iconifies) the window if the window loses focus additionally auto iconify restores the hardware resolution of the monitor if the window that loses focus is a fullscreen window
	glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	if (!monitor)
	{
		Fatal("No Monitor detected");
		return false;
	}
	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

	// Create the window
	handle = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
	if (!handle)
	{
		Fatal("Failed to create GLFW window");
		return false;
	}

	// Window callbacks
	//glfwSetWindowPosCallback(handle, windowPosCallback);
	glfwSetWindowFocusCallback(handle, windowFocusCallback);
	//glfwSetWindowIconifyCallback(handle, windowIconifyCallback);
	//glfwSetWindowMaximizeCallback(handle, windowMaximizeCallback);
	glfwSetFramebufferSizeCallback(handle, framebufferSizeCallback);
	//glfwSetWindowCloseCallback(handle, windowCloseCallback);
	glfwSetMonitorCallback(monitorCallback);

	// Key callbacks 
	glfwSetKeyCallback(handle, handleKeysCallback);
	glfwSetCharCallback(handle, handleCharCallback);

	// Mouse callbacks 
	glfwSetCursorEnterCallback(handle, handleCursorEnterCallback);
	glfwSetMouseButtonCallback(handle, handleMouseButtonCallback);
	glfwSetCursorPosCallback(handle, handleMouseCursorPosCallback);
	glfwSetScrollCallback(handle, handleMouseScrollCallback);

	// Get buffer size information
	int displayW, displayH;
	glfwGetFramebufferSize(handle, &displayW, &displayH);
	windowWidth = static_cast<uint16_t>(displayW);
	windowHeight = static_cast<uint16_t>(displayH);
	windowAspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

	int monitorLeft, monitorTop;
	glfwGetMonitorPos(monitor, &monitorLeft, &monitorTop);

	glfwSetWindowPos(handle,
		videoMode->width / 2 - windowWidth / 2 + monitorLeft,
		videoMode->height / 2 - windowHeight / 2 + monitorTop);

	// Set the current context
	glfwMakeContextCurrent(handle);
		
	// glad: load all OpenGL function pointers
	const int openGLVersion = gladLoadGL(glfwGetProcAddress);
	if (openGLVersion < GLAD_MAKE_VERSION(3, 3))
	{
		Fatal("Failed to initialize OpenGL context!");
		return false;
	}

	glfwSwapInterval(vsync ? 1 : 0);

	return true;
}
//=============================================================================
void window::Close() noexcept
{
	if (handle) glfwDestroyWindow(handle);
	handle = nullptr;
	glfwTerminate();
}
//=============================================================================
bool window::WindowShouldClose() noexcept
{
	return glfwWindowShouldClose(handle);
}
//=============================================================================
void window::Swap()
{
	glfwSwapBuffers(handle);
}
//=============================================================================
uint16_t window::GetWidth() noexcept { return windowWidth; }
uint16_t window::GetHeight() noexcept { return windowHeight; }
float window::GetAspect() noexcept { return windowAspect; }
//=============================================================================
void input::Init()
{
	double xpos, ypos;
	glfwGetCursorPos(window::handle, &xpos, &ypos);
	cursorOffset.x = static_cast<float>(xpos);
	cursorOffset.y = static_cast<float>(ypos);
	cursorPosLastFrame = cursorOffset;
}
//=============================================================================
void input::Update()
{
	for (unsigned i = 0; i < MaxKeys; ++i)
	{
		// keystates decay to either up or down after one frame
		if (keysStatus[i] & KeyState::Up)   keysStatus[i] = KeyState::Up;
		if (keysStatus[i] & KeyState::Down) keysStatus[i] = KeyState::Down;
	}
	for (unsigned i = 0; i < MaxMouseButtons; i++)
	{
		if (mouseButtonStatus[i] & KeyState::Up)   mouseButtonStatus[i] = KeyState::Up;
		if (mouseButtonStatus[i] & KeyState::Down) mouseButtonStatus[i] = KeyState::Down;
	}
	scrollOffset = glm::vec2(0);
	cursorOffset = glm::vec2(0);
	glfwPollEvents();
}
//=============================================================================
void input::SetCursorVisible(bool state)
{
	glfwSetInputMode(window::handle, GLFW_CURSOR, state ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}
//=============================================================================
const glm::vec2& input::GetCursorPos() noexcept { return cursorPos; }
const glm::vec2& input::GetCursorOffset() noexcept { return cursorOffset; }
const glm::vec2& input::GetPrevCursorPos() noexcept { return cursorPosLastFrame; }
const glm::vec2& input::GetScrollOffset() noexcept { return scrollOffset; }
//=============================================================================
bool input::IsKeyDown(Key key) noexcept { return keysStatus[key] & KeyState::Down; }
bool input::IsKeyUp(Key key) noexcept { return keysStatus[key] & KeyState::Up; }
bool input::IsKeyPressed(Key key) noexcept { return keysStatus[key] == KeyState::Pressed; }
bool input::IsKeyReleased(Key key) noexcept { return keysStatus[key] == KeyState::Released; }
bool input::IsMouseDown(MouseButton key) noexcept { return mouseButtonStatus[key] & KeyState::Down; }
bool input::IsMouseUp(MouseButton key) noexcept { return mouseButtonStatus[key] & KeyState::Up; }
bool input::IsMousePressed(MouseButton key) noexcept { return mouseButtonStatus[key] == KeyState::Pressed; }
bool input::IsMouseReleased(MouseButton key) noexcept { return mouseButtonStatus[key] == KeyState::Released; }
//=============================================================================