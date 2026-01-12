#pragma once

#ifdef __gl_h_
#error OpenGL (gl.h) header already included (API: gl), remove previous include!
#endif
#define __gl_h_

#if defined(APIENTRY)
#	define MIN_API_PTR APIENTRY
#elif defined(_WIN32)
#	define MIN_API_PTR __stdcall
#else
#	define MIN_API_PTR
#endif

template<typename T>
inline void MinGLLoadProc(T& func, const char* name)
{
	func = reinterpret_cast<T>(wglGetProcAddress(name));
}

//=========================================================
// OpenGL Base
//=========================================================

using GLsizei = int;
using GLuint = unsigned int;

#define GL_FALSE 0
#define GL_TRUE 1

//=========================================================
// OpenGL 1.0 Core
//=========================================================


//=========================================================
// WGL
//=========================================================
#if defined(_WIN32)

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126

inline HGLRC(MIN_API_PTR* wglCreateContextAttribsARB)(HDC hdc, HGLRC hglrc, const int* attribList) = nullptr;
inline HRESULT(MIN_API_PTR* wglChoosePixelFormatARB)(HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats) = nullptr;
inline BOOL(MIN_API_PTR* wglSwapIntervalEXT)(int interval) = nullptr;

inline void WGLLoad()
{
	MinGLLoadProc(wglCreateContextAttribsARB, "wglCreateContextAttribsARB");
	MinGLLoadProc(wglChoosePixelFormatARB, "wglChoosePixelFormatARB");
	MinGLLoadProc(wglSwapIntervalEXT, "wglSwapIntervalEXT");
}
#endif