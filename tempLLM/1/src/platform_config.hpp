#pragma once

#include <string>

// Определение платформы
#if defined(_WIN32) || defined(_WIN64)
    #define GL46_PLATFORM_WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
#elif defined(__linux__) || defined(__unix__)
    #define GL46_PLATFORM_LINUX
#elif defined(__APPLE__)
    #define GL46_PLATFORM_APPLE
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #define GL46_PLATFORM_IOS
    #else
        #define GL46_PLATFORM_MACOS
    #endif
#else
    #error "Unsupported platform"
#endif

// Определение компилятора
#if defined(__clang__)
    #define GL46_COMPILER_CLANG
    #define GL46_COMPILER_NAME "Clang"
#elif defined(__GNUC__)
    #define GL46_COMPILER_GCC
    #define GL46_COMPILER_NAME "GCC"
    #if __GNUC__ < 1
        #warning "GCC version < 11 may not support all C++23 features"
    #endif
#elif defined(_MSC_VER)
    #define GL46_COMPILER_MSVC
    #define GL46_COMPILER_NAME "MSVC"
    #if _MSC_VER < 1930
        #warning "MSVC version < 19.30 may not support all C++23 features"
    #endif
#else
    #define GL46_COMPILER_UNKNOWN
    #define GL46_COMPILER_NAME "Unknown"
    #warning "Unsupported compiler, assuming C++23 support"
#endif

// Определение архитектуры
#if defined(__x86_64__) || defined(_M_X64)
    #define GL46_ARCH_X64
    #define GL46_ARCH_NAME "x64"
#elif defined(__i386) || defined(_M_IX86)
    #define GL46_ARCH_X86
    #define GL46_ARCH_NAME "x86"
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define GL46_ARCH_ARM64
    #define GL46_ARCH_NAME "ARM64"
#elif defined(__arm__) || defined(_M_ARM)
    #define GL46_ARCH_ARM
    #define GL46_ARCH_NAME "ARM"
#else
    #define GL46_ARCH_UNKNOWN
    #define GL46_ARCH_NAME "Unknown"
#endif

// Макросы для экспорта/импорта библиотеки
#ifdef GL46_PLATFORM_WINDOWS
    #ifdef GL46_BUILDING_LIBRARY
        #define GL46_API __declspec(dllexport)
    #else
        #define GL46_API __declspec(dllimport)
    #endif
    #define GL46_CALL __cdecl
#else
    #define GL46_API
    #define GL46_CALL
#endif

// Утилиты для платформенной специфичной функциональности
namespace gl46::platform {

#ifdef GL46_PLATFORM_WINDOWS
    inline void SetDpiAwareness() {
        // Установка DPI awareness на Windows
        auto user32 = LoadLibraryA("user32.dll");
        if (user32) {
            auto setProcessDpiAwarenessContext = 
                reinterpret_cast<BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT)>(
                    GetProcAddress(user32, "SetProcessDpiAwarenessContext")
                );
            if (setProcessDpiAwarenessContext) {
                setProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            }
            FreeLibrary(user32);
        }
    }
    
    inline std::string GetPlatformInfo() {
        return "Windows";
    }
#elif defined(GL46_PLATFORM_LINUX)
    inline std::string GetPlatformInfo() {
        return "Linux";
    }
#elif defined(GL46_PLATFORM_MACOS)
    inline std::string GetPlatformInfo() {
        return "macOS";
    }
#elif defined(GL46_PLATFORM_IOS)
    inline std::string GetPlatformInfo() {
        return "iOS";
    }
#endif

    inline std::string GetCompilerInfo() {
        return GL46_COMPILER_NAME;
    }
    
    inline std::string GetArchitectureInfo() {
        return GL46_ARCH_NAME;
    }

} // namespace gl46::platform