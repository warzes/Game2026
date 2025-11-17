#pragma once

#include "../opengl_types.hpp"
#include "../functions.hpp"
#include "../exceptions.hpp"
#include <string>
#include <sstream>
#include <iostream>

namespace gl46::utils {

// Вспомогательная функция для преобразования перечислений в строки
inline std::string ToString(ComparisonFunc func) {
    switch (func) {
        case ComparisonFunc::Never: return "Never";
        case ComparisonFunc::Less: return "Less";
        case ComparisonFunc::Equal: return "Equal";
        case ComparisonFunc::LessEqual: return "LessEqual";
        case ComparisonFunc::Greater: return "Greater";
        case ComparisonFunc::NotEqual: return "NotEqual";
        case ComparisonFunc::GreaterEqual: return "GreaterEqual";
        case ComparisonFunc::Always: return "Always";
        default: return "Unknown ComparisonFunc";
    }
}

inline std::string ToString(BufferType type) {
    switch (type) {
        case BufferType::ArrayBuffer: return "ArrayBuffer";
        case BufferType::ElementArrayBuffer: return "ElementArrayBuffer";
        case BufferType::UniformBuffer: return "UniformBuffer";
        case BufferType::ShaderStorageBuffer: return "ShaderStorageBuffer";
        case BufferType::TextureBuffer: return "TextureBuffer";
        case BufferType::TransformFeedbackBuffer: return "TransformFeedbackBuffer";
        case BufferType::CopyReadBuffer: return "CopyReadBuffer";
        case BufferType::CopyWriteBuffer: return "CopyWriteBuffer";
        case BufferType::DispatchIndirectBuffer: return "DispatchIndirectBuffer";
        case BufferType::DrawIndirectBuffer: return "DrawIndirectBuffer";
        case BufferType::PixelPackBuffer: return "PixelPackBuffer";
        case BufferType::PixelUnpackBuffer: return "PixelUnpackBuffer";
        case BufferType::QueryBuffer: return "QueryBuffer";
        default: return "Unknown BufferType";
    }
}

inline std::string ToString(ShaderType type) {
    switch (type) {
        case ShaderType::VertexShader: return "VertexShader";
        case ShaderType::FragmentShader: return "FragmentShader";
        case ShaderType::GeometryShader: return "GeometryShader";
        case ShaderType::TessControlShader: return "TessControlShader";
        case ShaderType::TessEvaluationShader: return "TessEvaluationShader";
        case ShaderType::ComputeShader: return "ComputeShader";
        default: return "Unknown ShaderType";
    }
}

inline std::string ToString(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::Points: return "Points";
        case PrimitiveType::Lines: return "Lines";
        case PrimitiveType::LineLoop: return "LineLoop";
        case PrimitiveType::LineStrip: return "LineStrip";
        case PrimitiveType::Triangles: return "Triangles";
        case PrimitiveType::TriangleStrip: return "TriangleStrip";
        case PrimitiveType::TriangleFan: return "TriangleFan";
        case PrimitiveType::LinesAdjacency: return "LinesAdjacency";
        case PrimitiveType::LineStripAdjacency: return "LineStripAdjacency";
        case PrimitiveType::TrianglesAdjacency: return "TrianglesAdjacency";
        case PrimitiveType::TriangleStripAdjacency: return "TriangleStripAdjacency";
        case PrimitiveType::Patches: return "Patches";
        default: return "Unknown PrimitiveType";
    }
}

inline std::string ToString(ErrorCode error) {
    switch (error) {
        case ErrorCode::NoError: return "NoError";
        case ErrorCode::InvalidEnum: return "InvalidEnum";
        case ErrorCode::InvalidValue: return "InvalidValue";
        case ErrorCode::InvalidOperation: return "InvalidOperation";
        case ErrorCode::OutOfMemory: return "OutOfMemory";
        case ErrorCode::InvalidFramebufferOperation: return "InvalidFramebufferOperation";
        default: return "Unknown ErrorCode";
    }
}

// Класс для отладки OpenGL состояния
class DebugState {
public:
    static void PrintGLVersion() {
        const GLubyte* version = glGetString(GL_VERSION);
        const GLubyte* vendor = glGetString(GL_VENDOR);
        const GLubyte* renderer = glGetString(GL_RENDERER);
        
        std::cout << "OpenGL Version: " << (version ? reinterpret_cast<const char*>(version) : "Unknown") << std::endl;
        std::cout << "Vendor: " << (vendor ? reinterpret_cast<const char*>(vendor) : "Unknown") << std::endl;
        std::cout << "Renderer: " << (renderer ? reinterpret_cast<const char*>(renderer) : "Unknown") << std::endl;
    }
    
    static void PrintMaxValues() {
        GLint max_texture_size, max_viewport_dims[2], max_vertex_attribs;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
        glGetIntegerv(GL_MAX_VIEWPORT_DIMS, max_viewport_dims);
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);
        
        std::cout << "Max Texture Size: " << max_texture_size << std::endl;
        std::cout << "Max Viewport Dims: " << max_viewport_dims[0] << "x" << max_viewport_dims[1] << std::endl;
        std::cout << "Max Vertex Attributes: " << max_vertex_attribs << std::endl;
    }
    
    static void CheckAndPrintError(const std::string& operation) {
        auto error = GetError();
        if (error != ErrorCode::NoError) {
            std::cout << "OpenGL Error after '" << operation << "': " << ToString(error) << std::endl;
        }
    }
};

// Утилиты для профилирования
class Profiler {
private:
    static inline QueryHandle time_elapsed_query_{0};
    static inline bool initialized_{false};
    
public:
    static void Initialize() {
        if (!initialized_) {
            time_elapsed_query_ = CreateQuery();
            initialized_ = true;
        }
    }
    
    static void BeginTimeElapsedQuery() {
        if (initialized_) {
            BeginQuery(time_elapsed_query_, QueryTarget::TimeElapsed);
        }
    }
    
    static GLuint64 EndTimeElapsedQuery() {
        if (initialized_) {
            EndQuery(time_elapsed_query_, QueryTarget::TimeElapsed);
            
            GLuint64 time_elapsed = 0;
            GLint available = 0;
            do {
                GetQueryObjectiv(time_elapsed_query_, QueryParameterName::QueryResultAvailable, &available);
            } while (!available);
            
            GetQueryObjectui64v(time_elapsed_query_, QueryParameterName::QueryResult, &time_elapsed);
            return time_elapsed; // В наносекундах
        }
        return 0;
    }
    
    static void Shutdown() {
        if (initialized_ && time_elapsed_query_.valid()) {
            DestroyQuery(time_elapsed_query_);
            initialized_ = false;
        }
    }
};

} // namespace gl46::utils