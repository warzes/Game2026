#pragma once

#include "../opengl_types.hpp"
#include "../handles.hpp"
#include "../functions.hpp"
#include "../exceptions.hpp"

#ifdef GL46_ENABLE_VALIDATION
    #define GL46_VALIDATE(expr) do { expr; } while(0)
#else
    #define GL46_VALIDATE(expr) do { } while(0)
#endif

namespace gl46::validation {

class Validator {
public:
    static void CheckError(const char* operation) {
#ifdef GL46_ENABLE_VALIDATION
        auto error = GetError();
        if (error != ErrorCode::NoError) {
            throw exceptions::OpenGLException(
                std::string("OpenGL error after ") + operation + ": " + 
                std::to_string(static_cast<GLenum>(error)), error
            );
        }
#endif
    }
    
    template<typename HandleType>
    static void ValidateHandle(const HandleType& handle, const char* resource_type) {
#ifdef GL46_ENABLE_VALIDATION
        if (!handle.valid()) {
            throw exceptions::InvalidHandleException(resource_type);
        }
#endif
    }
    
    static void ValidateShaderCompilation(ShaderHandle shader) {
#ifdef GL46_ENABLE_VALIDATION
        GLint success = 0;
        GetShaderiv(shader, ShaderParameterName::CompileStatus, &success);
        if (success == 0) {
            char infoLog[1024];
            GLsizei length = 0;
            GetShaderInfoLog(shader, sizeof(infoLog), &length, infoLog);
            throw exceptions::ShaderCompilationException(std::string(infoLog, length));
        }
#endif
    }
    
    static void ValidateProgramLink(ProgramHandle program) {
#ifdef GL46_ENABLE_VALIDATION
        GLint success = 0;
        GetProgramiv(program, ProgramProperty::LinkStatus, &success);
        if (success == 0) {
            char infoLog[1024];
            GLsizei length = 0;
            GetProgramInfoLog(program, sizeof(infoLog), &length, infoLog);
            throw exceptions::ProgramLinkException(std::string(infoLog, length));
        }
#endif
    }
    
    static FramebufferStatus ValidateFramebufferStatus(FramebufferHandle framebuffer, FramebufferTarget target) {
#ifdef GL46_ENABLE_VALIDATION
        auto status = CheckNamedFramebufferStatus(framebuffer, target);
        if (status != FramebufferStatus::FramebufferComplete) {
            throw exceptions::FramebufferException(
                "Framebuffer is not complete: " + 
                std::to_string(static_cast<GLenum>(status))
            );
        }
        return status;
#else
        return CheckNamedFramebufferStatus(framebuffer, target);
#endif
    }
};

} // namespace gl46::validation