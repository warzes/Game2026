#pragma once

#include "opengl_types.hpp"
#include "handles.hpp"
#include "functions.hpp"
#include <string>
#include <vector>
#include <stdexcept>

namespace gl46 {

// Класс для инициализации OpenGL контекста
class Context {
public:
    static bool Initialize() {
        // Загрузка OpenGL функций через glad
        if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
            return false;
        }
        
        // Проверка версии OpenGL
        if (GLVersion.major < 4 || (GLVersion.major == 4 && GLVersion.minor < 6)) {
            return false;
        }
        
        return true;
    }
    
    static void SetViewport(int x, int y, int width, int height) {
        Viewport(x, y, width, height);
    }
    
    static void SetClearColor(float r, float g, float b, float a) {
        ClearColor(r, g, b, a);
    }
    
    static void Clear(ClearBufferMask mask) {
        gl46::Clear(mask);
    }
    
    static void Enable(EnableCap cap) {
        gl46::Enable(cap);
    }
    
    static void Disable(EnableCap cap) {
        gl46::Disable(cap);
    }
    
    static ErrorCode GetError() {
        return gl46::GetError();
    }
    
    static void CheckError(const std::string& operation) {
        auto error = GetError();
        if (error != ErrorCode::NoError) {
            throw std::runtime_error("OpenGL error after " + operation + ": " + std::to_string(static_cast<GLenum>(error)));
        }
    }
};

// RAII обертки для безопасной работы с ресурсами
class Buffer {
private:
    BufferHandle handle_;
    
public:
    Buffer() : handle_{0} {}
    
    Buffer(BufferType target, size_t size, const void* data = nullptr, BufferUsage usage = BufferUsage::StaticDraw) {
        handle_ = CreateBuffer(target, size, data, usage);
        if (!handle_.valid()) {
            throw std::runtime_error("Failed to create buffer");
        }
    }
    
    ~Buffer() {
        if (handle_.valid()) {
            DestroyBuffer(handle_);
        }
    }
    
    // Запрет копирования
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    
    // Разрешение перемещения
    Buffer(Buffer&& other) noexcept : handle_(other.handle_) {
        other.handle_ = BufferHandle{0};
    }
    
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            if (handle_.valid()) {
                DestroyBuffer(handle_);
            }
            handle_ = other.handle_;
            other.handle_ = BufferHandle{0};
        }
        return *this;
    }
    
    BufferHandle GetHandle() const { return handle_; }
    
    void SetData(size_t size, const void* data, BufferUsage usage = BufferUsage::StaticDraw) {
        SetBufferData(handle_, size, data, usage);
    }
    
    void SetSubData(size_t offset, size_t size, const void* data) {
        SetBufferSubData(handle_, offset, size, data);
    }
    
    void* Map(BufferAccess access) {
        return MapBuffer(handle_, access);
    }
    
    void* MapRange(size_t offset, size_t length, BufferAccessMask access) {
        return MapBufferRange(handle_, offset, length, access);
    }
    
    GLboolean Unmap() {
        return UnmapBuffer(handle_);
    }
};

class Texture {
private:
    TextureHandle handle_;
    TextureTarget target_;
    
public:
    Texture() : handle_{0}, target_(TextureTarget::Texture2D) {}
    
    Texture(TextureTarget target) : target_(target) {
        handle_ = CreateTexture(target);
        if (!handle_.valid()) {
            throw std::runtime_error("Failed to create texture");
        }
    }
    
    ~Texture() {
        if (handle_.valid()) {
            DestroyTexture(handle_);
        }
    }
    
    // Запрет копирования
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    
    // Разрешение перемещения
    Texture(Texture&& other) noexcept : handle_(other.handle_), target_(other.target_) {
        other.handle_ = TextureHandle{0};
        other.target_ = TextureTarget::Texture2D;
    }
    
    Texture& operator=(Texture&& other) noexcept {
        if (this != &other) {
            if (handle_.valid()) {
                DestroyTexture(handle_);
            }
            handle_ = other.handle_;
            target_ = other.target_;
            other.handle_ = TextureHandle{0};
            other.target_ = TextureTarget::Texture2D;
        }
        return *this;
    }
    
    TextureHandle GetHandle() const { return handle_; }
    
    void SetImage2D(
        GLint level,
        TextureInternalFormat internalFormat,
        GLsizei width,
        GLsizei height,
        GLint border,
        TextureFormat format,
        TextureDataType type,
        const void* data = nullptr
    ) {
        SetTextureImage2D(handle_, level, internalFormat, width, height, border, format, type, data);
    }
    
    void SetSubImage2D(
        GLint level,
        GLint xoffset,
        GLint yoffset,
        GLsizei width,
        GLsizei height,
        TextureFormat format,
        TextureDataType type,
        const void* pixels
    ) {
        SetTextureSubImage2D(handle_, level, xoffset, yoffset, width, height, format, type, pixels);
    }
    
    void GenerateMipmap() {
        GenerateTextureMipmap(handle_);
    }
    
    void SetParameter(TextureParameterName pname, GLint param) {
        SetTextureParameter(handle_, pname, param);
    }
    
    void SetParameter(TextureParameterName pname, GLfloat param) {
        SetTextureParameter(handle_, pname, param);
    }
};

class Shader {
private:
    ShaderHandle handle_;
    ShaderType type_;
    
public:
    Shader() : handle_{0}, type_(ShaderType::VertexShader) {}
    
    Shader(ShaderType type, const std::string& source) : type_(type) {
        handle_ = CreateShader(type);
        if (!handle_.valid()) {
            throw std::runtime_error("Failed to create shader");
        }
        
        SetShaderSource(handle_, source.c_str());
        CompileShader(handle_);
        
        // Проверка ошибок компиляции
        GLint success = 0;
        GetShaderiv(handle_, ShaderParameterName::CompileStatus, &success);
        if (success == 0) {
            char infoLog[512];
            GetShaderInfoLog(handle_, 512, nullptr, infoLog);
            std::string errorMsg = "Shader compilation failed: " + std::string(infoLog);
            DestroyShader(handle_);
            handle_ = ShaderHandle{0};
            throw std::runtime_error(errorMsg);
        }
    }
    
    ~Shader() {
        if (handle_.valid()) {
            DestroyShader(handle_);
        }
    }
    
    // Запрет копирования
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    // Разрешение перемещения
    Shader(Shader&& other) noexcept : handle_(other.handle_), type_(other.type_) {
        other.handle_ = ShaderHandle{0};
        other.type_ = ShaderType::VertexShader;
    }
    
    Shader& operator=(Shader&& other) noexcept {
        if (this != &other) {
            if (handle_.valid()) {
                DestroyShader(handle_);
            }
            handle_ = other.handle_;
            type_ = other.type_;
            other.handle_ = ShaderHandle{0};
            other.type_ = ShaderType::VertexShader;
        }
        return *this;
    }
    
    ShaderHandle GetHandle() const { return handle_; }
};

class Program {
private:
    ProgramHandle handle_;
    
public:
    Program() : handle_{0} {}
    
    Program(const std::vector<ShaderHandle>& shaders) {
        handle_ = CreateProgram();
        if (!handle_.valid()) {
            throw std::runtime_error("Failed to create program");
        }
        
        for (auto shader : shaders) {
            AttachShader(handle_, shader);
        }
        
        LinkProgram(handle_);
        
        // Проверка ошибок линковки
        GLint success = 0;
        GetProgramiv(handle_, ProgramProperty::LinkStatus, &success);
        if (success == 0) {
            char infoLog[512];
            GetProgramInfoLog(handle_, 512, nullptr, infoLog);
            std::string errorMsg = "Program linking failed: " + std::string(infoLog);
            DestroyProgram(handle_);
            handle_ = ProgramHandle{0};
            throw std::runtime_error(errorMsg);
        }
    }
    
    ~Program() {
        if (handle_.valid()) {
            DestroyProgram(handle_);
        }
    }
    
    // Запрет копирования
    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;
    
    // Разрешение перемещения
    Program(Program&& other) noexcept : handle_(other.handle_) {
        other.handle_ = ProgramHandle{0};
    }
    
    Program& operator=(Program&& other) noexcept {
        if (this != &other) {
            if (handle_.valid()) {
                DestroyProgram(handle_);
            }
            handle_ = other.handle_;
            other.handle_ = ProgramHandle{0};
        }
        return *this;
    }
    
    ProgramHandle GetHandle() const { return handle_; }
    
    void Use() {
        UseProgram(handle_);
    }
    
    GLint GetUniformLocation(const char* name) {
        return glGetUniformLocation(handle_, name);
    }
    
    void SetUniform1f(const char* name, GLfloat v0) {
        auto location = GetUniformLocation(name);
        SetUniform1f(handle_, location, v0);
    }
    
    void SetUniform2f(const char* name, GLfloat v0, GLfloat v1) {
        auto location = GetUniformLocation(name);
        SetUniform2f(handle_, location, v0, v1);
    }
    
    void SetUniform3f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2) {
        auto location = GetUniformLocation(name);
        SetUniform3f(handle_, location, v0, v1, v2);
    }
    
    void SetUniform4f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
        auto location = GetUniformLocation(name);
        SetUniform4f(handle_, location, v0, v1, v2, v3);
    }
    
    void SetUniform1i(const char* name, GLint v0) {
        auto location = GetUniformLocation(name);
        SetUniform1i(handle_, location, v0);
    }
    
    void SetUniform2i(const char* name, GLint v0, GLint v1) {
        auto location = GetUniformLocation(name);
        SetUniform2i(handle_, location, v0, v1);
    }
    
    void SetUniform3i(const char* name, GLint v0, GLint v1, GLint v2) {
        auto location = GetUniformLocation(name);
        SetUniform3i(handle_, location, v0, v1, v2);
    }
    
    void SetUniform4i(const char* name, GLint v0, GLint v1, GLint v2, GLint v3) {
        auto location = GetUniformLocation(name);
        SetUniform4i(handle_, location, v0, v1, v2, v3);
    }
    
    void SetUniformMatrix4fv(const char* name, GLsizei count, GLboolean transpose, const GLfloat* value) {
        auto location = GetUniformLocation(name);
        SetUniformMatrix4fv(handle_, location, count, transpose, value);
    }
};

} // namespace gl46