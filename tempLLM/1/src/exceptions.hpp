#pragma once

#include "opengl_types.hpp"
#include <stdexcept>
#include <string>

namespace gl46::exceptions {

class OpenGLException : public std::exception {
protected:
    std::string message_;
    ErrorCode error_code_{ErrorCode::NoError};
    
public:
    OpenGLException(const std::string& msg, ErrorCode code = ErrorCode::NoError) 
        : message_(msg), error_code_(code) {}
    
    const char* what() const noexcept override { return message_.c_str(); }
    ErrorCode GetErrorCode() const noexcept { return error_code_; }
};

class BufferCreationException : public OpenGLException {
public:
    BufferCreationException(const std::string& msg) 
        : OpenGLException("Buffer creation failed: " + msg) {}
};

class ShaderCompilationException : public OpenGLException {
public:
    ShaderCompilationException(const std::string& log) 
        : OpenGLException("Shader compilation failed: " + log) {}
};

class ProgramLinkException : public OpenGLException {
public:
    ProgramLinkException(const std::string& log) 
        : OpenGLException("Program linking failed: " + log) {}
};

class TextureCreationException : public OpenGLException {
public:
    TextureCreationException(const std::string& msg) 
        : OpenGLException("Texture creation failed: " + msg) {}
};

class FramebufferException : public OpenGLException {
public:
    FramebufferException(const std::string& msg) 
        : OpenGLException("Framebuffer operation failed: " + msg) {}
};

class InvalidHandleException : public OpenGLException {
public:
    InvalidHandleException(const std::string& resource_type) 
        : OpenGLException("Invalid " + resource_type + " handle") {}
};

} // namespace gl46::exceptions