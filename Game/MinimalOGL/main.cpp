#if defined(_WIN32)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#endif
#include "MinOGL.h"

#pragma comment(lib, "opengl32.lib")

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	WNDCLASS wndclass = { 0, DefWindowProc, 0, 0, 0, 0, 0, 0, 0, L"MinOpenGL" };
	RegisterClass(&wndclass);

	PIXELFORMATDESCRIPTOR pfd = { sizeof(pfd), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 32, 8, PFD_MAIN_PLANE, 32, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 32, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

	HWND dummyWin = CreateWindow(wndclass.lpszClassName, L"Dummy", 0, 0, 0, 0, 0, 0, 0, 0, 0);
	HDC dummy_dc = GetDC(dummyWin);
	int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
	SetPixelFormat(dummy_dc, pixel_format, &pfd);	
	HGLRC dummy_context = wglCreateContext(dummy_dc);
	wglMakeCurrent(dummy_dc, dummy_context);

	WGLLoad();

	wglMakeCurrent(dummy_dc, 0);
	wglDeleteContext(dummy_context);
	ReleaseDC(dummyWin, dummy_dc);
	DestroyWindow(dummyWin);

	// windows makes you define the macros yourself, so I'll be using the hardcoded instead 
	static int attribs[] = {
		0x2003, // WGL_ACCELERATION_ARB
		0x2027, // WGL_FULL_ACCELERATION_ARB
		0x201b, 8, // WGL_ALPHA_BITS_ARB
		0x2022, 24, // WGL_DEPTH_BITS_ARB
		0x2001, 1, // WGL_DRAW_TO_WINDOW_ARB
		0x2015, 8, // WGL_RED_BITS_ARB
		0x2017, 8, // WGL_GREEN_BITS_ARB
		0x2019, 8, // WGL_BLUE_BITS_ARB
		0x2013, 0x202B, // WGL_PIXEL_TYPE_ARB,  WGL_TYPE_RGBA_ARB
		0x2010,		1, // WGL_SUPPORT_OPENGL_ARB
		0x2014,	 32, // WGL_COLOR_BITS_ARB
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	size_t index = (sizeof(attribs) / sizeof(attribs[0])) - 13;
#define RGFW_GL_ADD_ATTRIB(attrib, attVal) \
if (attVal) { \
attribs[index] = attrib;\
attribs[index + 1] = attVal;\
index += 2;\
}

	int32_t RGFW_STENCIL = 0, RGFW_SAMPLES = 0, RGFW_STEREO = GL_FALSE, RGFW_AUX_BUFFERS = 0, RGFW_DOUBLE_BUFFER = 1;

	if (RGFW_DOUBLE_BUFFER)
		RGFW_GL_ADD_ATTRIB(0x2011, 1);  // double buffer
	RGFW_GL_ADD_ATTRIB(0x2023, RGFW_STENCIL);
	RGFW_GL_ADD_ATTRIB(0x2012, RGFW_STEREO);
	RGFW_GL_ADD_ATTRIB(0x2024, RGFW_AUX_BUFFERS);
	RGFW_GL_ADD_ATTRIB(0x2042, RGFW_SAMPLES);
	RGFW_GL_ADD_ATTRIB(0, 0);

	typedef uint8_t RGFW_GL_profile; enum { RGFW_GL_CORE = 0, RGFW_GL_COMPATIBILITY };
	int32_t RGFW_majorVersion = 0, RGFW_minorVersion = 0;
	uint8_t RGFW_profile = RGFW_GL_CORE;

	 HWND window = CreateWindowEx(0, wndclass.lpszClassName, L"OpenGL", WS_POPUP | WS_MAXIMIZE | WS_VISIBLE, 0, 0, 800, 600, nullptr, nullptr, nullptr, nullptr);
	HDC hdc = GetDC(window);
	int pixel_format2;
	UINT num_formats;
	wglChoosePixelFormatARB(hdc, attribs, 0, 1, &pixel_format2, &num_formats);

	DescribePixelFormat(hdc, pixel_format2, sizeof(pfd), &pfd);
	SetPixelFormat(hdc, pixel_format2, &pfd);

#define SET_ATTRIB(a, v) { \
assert(((size_t) index + 1) < sizeof(context_attribs) / sizeof(context_attribs[0])); \
context_attribs[index++] = a; \
context_attribs[index++] = v; \
}

	/* create opengl/WGL context for the specified version */
	index = 0;
	int32_t context_attribs[40];

	if (RGFW_profile == RGFW_GL_CORE) {
		SET_ATTRIB(WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB);
	}
	else {
		SET_ATTRIB(WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB);
	}

	if (RGFW_majorVersion || RGFW_minorVersion) {
		SET_ATTRIB(WGL_CONTEXT_MAJOR_VERSION_ARB, RGFW_majorVersion);
		SET_ATTRIB(WGL_CONTEXT_MINOR_VERSION_ARB, RGFW_minorVersion);
	}

	SET_ATTRIB(0, 0);

	HGLRC ctx = (HGLRC)wglCreateContextAttribsARB(hdc, NULL, context_attribs);
	wglMakeCurrent(hdc, ctx);

	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);

	MSG msg;

	BOOL running = TRUE;
	while (running)
	{
		if (PeekMessage(&msg, window, 0u, 0u, PM_REMOVE))
		{
			switch (msg.message) {
			case WM_CLOSE:
			case WM_QUIT:
				running = FALSE;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		running = IsWindow(window);

		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);

		SwapBuffers(hdc);
	}

	DeleteDC(hdc);
	DestroyWindow(window);
	return 0;
}