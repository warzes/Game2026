#pragma once

// Основной заголовочный файл для всей библиотеки OpenGL 4.6 Type-Safe Wrapper
// Включает все необходимые компоненты

#include "opengl_types.hpp"
#include "handles.hpp"
#include "functions.hpp"
#include "core.hpp"

// Также включаем заголовочные файлы для специфичных компонентов
#include "buffer.hpp"
#include "vertex_array.hpp"
#include "shader.hpp"
#include "program.hpp"
#include "texture.hpp"
#include "framebuffer.hpp"
#include "renderbuffer.hpp"
#include "pipeline.hpp"
#include "query.hpp"
#include "sampler.hpp"
#include "transform_feedback.hpp"
#include "vertex_specification.hpp"

// Пространство имен для всей библиотеки
namespace gl46 {

// Основные функции инициализации
inline bool Initialize() {
    // В реальной реализации здесь будет вызов загрузчика OpenGL функций
    // и проверка совместимости с OpenGL 4.6
    return true;
}

// Вспомогательная функция для проверки версии OpenGL
inline bool IsVersionSupported(int major, int minor) {
    GLint versionMajor, versionMinor;
    glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
    glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
    
    if (versionMajor > major || (versionMajor == major && versionMinor >= minor)) {
        return true;
    }
    return false;
}

} // namespace gl46