#pragma once

#include "opengl_types.hpp"
#include "handles.hpp"
#include <vector>
#include <cstring> // для memcpy

// Заголовочный файл для загрузки OpenGL функций (GLEW, glad или другой)
#include <glad/glad.h> // Используем glad по умолчанию

namespace gl46 { // Используем gl46 как префикс для OpenGL 4.6

// Функции для буферов
inline BufferHandle CreateBuffer(BufferType target, size_t size, const void* data, BufferUsage usage) {
    BufferHandle buffer;
    glCreateBuffers(1, &buffer.handle);
    glNamedBufferData(buffer.handle, static_cast<GLsizeiptr>(size), data, static_cast<GLenum>(usage));
    return buffer;
}

inline void SetBufferData(BufferHandle buffer, size_t size, const void* data, BufferUsage usage) {
    glNamedBufferData(buffer.handle, static_cast<GLsizeiptr>(size), data, static_cast<GLenum>(usage));
}

inline void SetBufferSubData(BufferHandle buffer, size_t offset, size_t size, const void* data) {
    glNamedBufferSubData(buffer.handle, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
}

inline void* MapBuffer(BufferHandle buffer, BufferAccess access) {
    return glMapNamedBuffer(buffer.handle, static_cast<GLenum>(access));
}

inline void* MapBufferRange(BufferHandle buffer, size_t offset, size_t length, BufferAccessMask access) {
    return glMapNamedBufferRange(buffer.handle, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(length), static_cast<GLbitfield>(access));
}

inline GLboolean UnmapBuffer(BufferHandle buffer) {
    return glUnmapNamedBuffer(buffer.handle);
}

inline void DestroyBuffer(BufferHandle buffer) {
    if (buffer.valid()) {
        GLuint handle = buffer.handle;
        glDeleteBuffers(1, &handle);
    }
}

// Функции для текстур
inline TextureHandle CreateTexture(TextureTarget target) {
    TextureHandle texture;
    glCreateTextures(static_cast<GLenum>(target), 1, &texture.handle);
    return texture;
}

inline void SetTextureImage2D(
    TextureHandle texture,
    GLint level,
    TextureInternalFormat internalFormat,
    GLsizei width,
    GLsizei height,
    GLint border,
    TextureFormat format,
    TextureDataType type,
    const void* data
) {
    glTextureImage2D(
        texture.handle,
        level,
        static_cast<GLenum>(internalFormat),
        width,
        height,
        border,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data
    );
}

inline void SetTextureSubImage2D(
    TextureHandle texture,
    GLint level,
    GLint xoffset,
    GLint yoffset,
    GLsizei width,
    GLsizei height,
    TextureFormat format,
    TextureDataType type,
    const void* pixels
) {
    glTextureSubImage2D(
        texture.handle,
        level,
        xoffset,
        yoffset,
        width,
        height,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels
    );
}

inline void GenerateTextureMipmap(TextureHandle texture) {
    glGenerateTextureMipmap(texture.handle);
}

inline void SetTextureParameter(TextureHandle texture, TextureParameterName pname, GLint param) {
    glTextureParameteri(texture.handle, static_cast<GLenum>(pname), param);
}

inline void SetTextureParameter(TextureHandle texture, TextureParameterName pname, GLfloat param) {
    glTextureParameterf(texture.handle, static_cast<GLenum>(pname), param);
}

inline void DestroyTexture(TextureHandle texture) {
    if (texture.valid()) {
        GLuint handle = texture.handle;
        glDeleteTextures(1, &handle);
    }
}

// Функции для шейдеров
inline ShaderHandle CreateShader(ShaderType type) {
    ShaderHandle shader;
    shader.handle = glCreateShader(static_cast<GLenum>(type));
    return shader;
}

inline void SetShaderSource(ShaderHandle shader, const char* source) {
    glShaderSource(shader.handle, 1, &source, nullptr);
}

inline void CompileShader(ShaderHandle shader) {
    glCompileShader(shader.handle);
}

inline void GetShaderiv(ShaderHandle shader, ShaderParameterName pname, GLint* params) {
    glGetShaderiv(shader.handle, static_cast<GLenum>(pname), params);
}

inline void GetShaderInfoLog(ShaderHandle shader, GLsizei bufSize, GLsizei* length, char* infoLog) {
    glGetShaderInfoLog(shader.handle, bufSize, length, infoLog);
}

inline void DestroyShader(ShaderHandle shader) {
    if (shader.valid()) {
        glDeleteShader(shader.handle);
    }
}

// Функции для программ
inline ProgramHandle CreateProgram() {
    ProgramHandle program;
    program.handle = glCreateProgram();
    return program;
}

inline void AttachShader(ProgramHandle program, ShaderHandle shader) {
    glAttachShader(program.handle, shader.handle);
}

inline void LinkProgram(ProgramHandle program) {
    glLinkProgram(program.handle);
}

inline void UseProgram(ProgramHandle program) {
    glUseProgram(program.handle);
}

inline void GetProgramiv(ProgramHandle program, ProgramProperty pname, GLint* params) {
    glGetProgramiv(program.handle, static_cast<GLenum>(pname), params);
}

inline void GetProgramInfoLog(ProgramHandle program, GLsizei bufSize, GLsizei* length, char* infoLog) {
    glGetProgramInfoLog(program.handle, bufSize, length, infoLog);
}

inline GLint GetUniformLocation(ProgramHandle program, const char* name) {
    return glGetUniformLocation(program.handle, name);
}

inline void SetUniform1f(ProgramHandle program, GLint location, GLfloat v0) {
    glUniform1f(location, v0);
}

inline void SetUniform2f(ProgramHandle program, GLint location, GLfloat v0, GLfloat v1) {
    glUniform2f(location, v0, v1);
}

inline void SetUniform3f(ProgramHandle program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2) {
    glUniform3f(location, v0, v1, v2);
}

inline void SetUniform4f(ProgramHandle program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
    glUniform4f(location, v0, v1, v2, v3);
}

inline void SetUniform1i(ProgramHandle program, GLint location, GLint v0) {
    glUniform1i(location, v0);
}

inline void SetUniform2i(ProgramHandle program, GLint location, GLint v0, GLint v1) {
    glUniform2i(location, v0, v1);
}

inline void SetUniform3i(ProgramHandle program, GLint location, GLint v0, GLint v1, GLint v2) {
    glUniform3i(location, v0, v1, v2);
}

inline void SetUniform4i(ProgramHandle program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3) {
    glUniform4i(location, v0, v1, v2, v3);
}

inline void SetUniformMatrix4fv(ProgramHandle program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) {
    glUniformMatrix4fv(location, count, transpose, value);
}

inline void DestroyProgram(ProgramHandle program) {
    if (program.valid()) {
        glDeleteProgram(program.handle);
    }
}

// Функции для вершинных массивов
inline VertexArrayHandle CreateVertexArray() {
    VertexArrayHandle vao;
    glCreateVertexArrays(1, &vao.handle);
    return vao;
}

inline void BindVertexArray(VertexArrayHandle vao) {
    glBindVertexArray(vao.handle);
}

inline void EnableVertexArrayAttrib(VertexArrayHandle vao, VertexAttribBinding index) {
    glEnableVertexArrayAttrib(vao.handle, index.index);
}

inline void DisableVertexArrayAttrib(VertexArrayHandle vao, VertexAttribBinding index) {
    glDisableVertexArrayAttrib(vao.handle, index.index);
}

inline void VertexArrayAttribFormat(
    VertexArrayHandle vao,
    VertexAttribBinding attribIndex,
    GLint size,
    DataType type,
    GLboolean normalized,
    GLuint relativeOffset
) {
    glVertexArrayAttribFormat(vao.handle, attribIndex.index, size, static_cast<GLenum>(type), normalized, relativeOffset);
}

inline void VertexArrayAttribIFormat(
    VertexArrayHandle vao,
    VertexAttribBinding attribIndex,
    GLint size,
    DataType type,
    GLuint relativeOffset
) {
    glVertexArrayAttribIFormat(vao.handle, attribIndex.index, size, static_cast<GLenum>(type), relativeOffset);
}

inline void VertexArrayAttribLFormat(
    VertexArrayHandle vao,
    VertexAttribBinding attribIndex,
    GLint size,
    DataType type,
    GLuint relativeOffset
) {
    glVertexArrayAttribLFormat(vao.handle, attribIndex.index, size, static_cast<GLenum>(type), relativeOffset);
}

inline void VertexArrayBindingDivisor(VertexArrayHandle vao, VertexBufferBinding bindingIndex, GLuint divisor) {
    glVertexArrayBindingDivisor(vao.handle, bindingIndex.index, divisor);
}

inline void VertexArrayVertexBuffer(
    VertexArrayHandle vao,
    VertexBufferBinding bindingIndex,
    BufferHandle buffer,
    GLintptr offset,
    GLsizei stride
) {
    glVertexArrayVertexBuffer(vao.handle, bindingIndex.index, buffer.handle, offset, stride);
}

inline void VertexArrayVertexBuffers(
    VertexArrayHandle vao,
    GLuint first,
    GLsizei count,
    const BufferHandle* buffers,
    const GLintptr* offsets,
    const GLsizei* strides
) {
    // Извлекаем GLuint handle'ы из буферов для передачи в OpenGL
    std::vector<GLuint> bufferHandles(count);
    for (int i = 0; i < count; ++i) {
        bufferHandles[i] = buffers[i].handle;
    }
    glVertexArrayVertexBuffers(vao.handle, first, count, bufferHandles.data(), offsets, strides);
}

inline void VertexArrayElementBuffer(VertexArrayHandle vao, BufferHandle buffer) {
    glVertexArrayElementBuffer(vao.handle, buffer.handle);
}

inline void DestroyVertexArray(VertexArrayHandle vao) {
    if (vao.valid()) {
        GLuint handle = vao.handle;
        glDeleteVertexArrays(1, &handle);
    }
}

// Функции для фреймбуферов
inline FramebufferHandle CreateFramebuffer() {
    FramebufferHandle fbo;
    glCreateFramebuffers(1, &fbo.handle);
    return fbo;
}

inline void BindFramebuffer(FramebufferTarget target, FramebufferHandle framebuffer) {
    glBindFramebuffer(static_cast<GLenum>(target), framebuffer.handle);
}

inline void NamedFramebufferTexture(
    FramebufferHandle framebuffer,
    FramebufferAttachment attachment,
    TextureHandle texture,
    GLint level
) {
    glNamedFramebufferTexture(framebuffer.handle, static_cast<GLenum>(attachment), texture.handle, level);
}

inline void NamedFramebufferRenderbuffer(
    FramebufferHandle framebuffer,
    FramebufferAttachment attachment,
    RenderbufferTarget target,
    RenderbufferHandle renderbuffer
) {
    glNamedFramebufferRenderbuffer(framebuffer.handle, static_cast<GLenum>(attachment), static_cast<GLenum>(target), renderbuffer.handle);
}

inline FramebufferStatus CheckNamedFramebufferStatus(FramebufferHandle framebuffer, FramebufferTarget target) {
    auto status = glCheckNamedFramebufferStatus(framebuffer.handle, static_cast<GLenum>(target));
    return static_cast<FramebufferStatus>(status);
}

inline void DestroyFramebuffer(FramebufferHandle framebuffer) {
    if (framebuffer.valid()) {
        GLuint handle = framebuffer.handle;
        glDeleteFramebuffers(1, &handle);
    }
}

// Функции для рендербуферов
inline RenderbufferHandle CreateRenderbuffer() {
    RenderbufferHandle rbo;
    glCreateRenderbuffers(1, &rbo.handle);
    return rbo;
}

inline void NamedRenderbufferStorage(
    RenderbufferHandle renderbuffer,
    TextureInternalFormat internalformat,
    GLsizei width,
    GLsizei height
) {
    glNamedRenderbufferStorage(renderbuffer.handle, static_cast<GLenum>(internalformat), width, height);
}

inline void NamedRenderbufferStorageMultisample(
    RenderbufferHandle renderbuffer,
    GLsizei samples,
    TextureInternalFormat internalformat,
    GLsizei width,
    GLsizei height
) {
    glNamedRenderbufferStorageMultisample(renderbuffer.handle, samples, static_cast<GLenum>(internalformat), width, height);
}

inline void DestroyRenderbuffer(RenderbufferHandle renderbuffer) {
    if (renderbuffer.valid()) {
        GLuint handle = renderbuffer.handle;
        glDeleteRenderbuffers(1, &handle);
    }
}

// Функции для сэмплеров
inline SamplerHandle CreateSampler() {
    SamplerHandle sampler;
    glCreateSamplers(1, &sampler.handle);
    return sampler;
}

inline void BindSampler(GLuint unit, SamplerHandle sampler) {
    glBindSampler(unit, sampler.handle);
}

inline void SamplerParameter(SamplerHandle sampler, TextureParameterName pname, GLint param) {
    glSamplerParameteri(sampler.handle, static_cast<GLenum>(pname), param);
}

inline void SamplerParameter(SamplerHandle sampler, TextureParameterName pname, GLfloat param) {
    glSamplerParameterf(sampler.handle, static_cast<GLenum>(pname), param);
}

inline void DestroySampler(SamplerHandle sampler) {
    if (sampler.valid()) {
        GLuint handle = sampler.handle;
        glDeleteSamplers(1, &handle);
    }
}

// Функции для трансформ-фидбека
inline TransformFeedbackHandle CreateTransformFeedback() {
    TransformFeedbackHandle tfo;
    glCreateTransformFeedbacks(1, &tfo.handle);
    return tfo;
}

inline void BindTransformFeedback(TransformFeedbackTarget target, TransformFeedbackHandle tfo) {
    glBindTransformFeedback(static_cast<GLenum>(target), tfo.handle);
}

inline void BeginTransformFeedback(TransformFeedbackHandle tfo, PrimitiveType primitiveMode) {
    glBeginTransformFeedback(static_cast<GLenum>(primitiveMode));
}

inline void EndTransformFeedback(TransformFeedbackHandle tfo) {
    glEndTransformFeedback();
}

inline void PauseTransformFeedback(TransformFeedbackHandle tfo) {
    glPauseTransformFeedback();
}

inline void ResumeTransformFeedback(TransformFeedbackHandle tfo) {
    glResumeTransformFeedback();
}

inline void TransformFeedbackBufferBase(TransformFeedbackHandle tfo, GLuint index, BufferHandle buffer) {
    glTransformFeedbackBufferBase(tfo.handle, index, buffer.handle);
}

inline void TransformFeedbackBufferRange(TransformFeedbackHandle tfo, GLuint index, BufferHandle buffer, GLintptr offset, GLsizeiptr size) {
    glTransformFeedbackBufferRange(tfo.handle, index, buffer.handle, offset, size);
}

inline void DestroyTransformFeedback(TransformFeedbackHandle tfo) {
    if (tfo.valid()) {
        GLuint handle = tfo.handle;
        glDeleteTransformFeedbacks(1, &handle);
    }
}

// Функции для квери
inline QueryHandle CreateQuery() {
    QueryHandle query;
    glCreateQueries(static_cast<GLenum>(QueryTarget::SamplesPassed), 1, &query.handle);
    return query;
}

inline void BeginQuery(QueryHandle query, QueryTarget target) {
    glBeginQuery(static_cast<GLenum>(target), query.handle);
}

inline void EndQuery(QueryHandle query, QueryTarget target) {
    glEndQuery(static_cast<GLenum>(target));
}

inline void GetQueryObjectuiv(QueryHandle query, QueryParameterName pname, GLuint* params) {
    glGetQueryObjectuiv(query.handle, static_cast<GLenum>(pname), params);
}

inline void DestroyQuery(QueryHandle query) {
    if (query.valid()) {
        GLuint handle = query.handle;
        glDeleteQueries(1, &handle);
    }
}

// Функции для пайплайнов
inline PipelineHandle CreatePipeline() {
    PipelineHandle pipeline;
    glCreateProgramPipelines(1, &pipeline.handle);
    return pipeline;
}

inline void UseProgramStages(PipelineHandle pipeline, ShaderStageBitfield stages, ProgramHandle program) {
    glUseProgramStages(pipeline.handle, static_cast<GLbitfield>(stages), program.handle);
}

inline void ActiveShaderProgram(PipelineHandle pipeline, ProgramHandle program) {
    glActiveShaderProgram(pipeline.handle, program.handle);
}

inline void BindProgramPipeline(PipelineHandle pipeline) {
    glBindProgramPipeline(pipeline.handle);
}

inline void ValidateProgramPipeline(PipelineHandle pipeline) {
    glValidateProgramPipeline(pipeline.handle);
}

inline void DestroyPipeline(PipelineHandle pipeline) {
    if (pipeline.valid()) {
        GLuint handle = pipeline.handle;
        glDeleteProgramPipelines(1, &handle);
    }
}

// Функции рендеринга
inline void DrawArrays(PrimitiveType mode, GLint first, GLsizei count) {
    glDrawArrays(static_cast<GLenum>(mode), first, count);
}

inline void DrawElements(PrimitiveType mode, GLsizei count, DataType type, const void* indices) {
    glDrawElements(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), indices);
}

inline void DrawElementsBaseVertex(PrimitiveType mode, GLsizei count, DataType type, const void* indices, GLint basevertex) {
    glDrawElementsBaseVertex(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), indices, basevertex);
}

inline void DrawArraysInstanced(PrimitiveType mode, GLint first, GLsizei count, GLsizei instancecount) {
    glDrawArraysInstanced(static_cast<GLenum>(mode), first, count, instancecount);
}

inline void DrawElementsInstanced(PrimitiveType mode, GLsizei count, DataType type, const void* indices, GLsizei instancecount) {
    glDrawElementsInstanced(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), indices, instancecount);
}

inline void DrawElementsInstancedBaseVertex(PrimitiveType mode, GLsizei count, DataType type, const void* indices, GLsizei instancecount, GLint basevertex) {
    glDrawElementsInstancedBaseVertex(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), indices, instancecount, basevertex);
}

inline void DrawArraysInstancedBaseInstance(PrimitiveType mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance) {
    glDrawArraysInstancedBaseInstance(static_cast<GLenum>(mode), first, count, instancecount, baseinstance);
}

inline void DrawElementsInstancedBaseVertexBaseInstance(PrimitiveType mode, GLsizei count, DataType type, const void* indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance) {
    glDrawElementsInstancedBaseVertexBaseInstance(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), indices, instancecount, basevertex, baseinstance);
}

// Функции очистки буферов
inline void Clear(ClearBufferMask mask) {
    glClear(static_cast<GLbitfield>(mask));
}

inline void ClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    glClearColor(r, g, b, a);
}

inline void ClearDepth(GLdouble depth) {
    glClearDepth(depth);
}

inline void ClearStencil(GLint s) {
    glClearStencil(s);
}

// Функции настройки растеризатора
inline void Enable(EnableCap cap) {
    glEnable(static_cast<GLenum>(cap));
}

inline void Disable(EnableCap cap) {
    glDisable(static_cast<GLenum>(cap));
}

inline void CullFace(CullFaceMode mode) {
    glCullFace(static_cast<GLenum>(mode));
}

inline void FrontFace(FrontFaceMode mode) {
    glFrontFace(static_cast<GLenum>(mode));
}

inline void PolygonMode(FrontFaceMode face, PolygonMode mode) {
    glPolygonMode(static_cast<GLenum>(face), static_cast<GLenum>(mode));
}

// Функции настройки blending'а
inline void BlendFunc(BlendFactor sfactor, BlendFactor dfactor) {
    glBlendFunc(static_cast<GLenum>(sfactor), static_cast<GLenum>(dfactor));
}

inline void BlendEquation(BlendEquation mode) {
    glBlendEquation(static_cast<GLenum>(mode));
}

inline void BlendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha) {
    glBlendFuncSeparate(static_cast<GLenum>(srcRGB), static_cast<GLenum>(dstRGB), static_cast<GLenum>(srcAlpha), static_cast<GLenum>(dstAlpha));
}

inline void BlendEquationSeparate(BlendEquation modeRGB, BlendEquation modeAlpha) {
    glBlendEquationSeparate(static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
}

// Функции для работы с окном просмотра
inline void Viewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    glViewport(x, y, width, height);
}

inline void Scissor(GLint x, GLint y, GLsizei width, GLsizei height) {
    glScissor(x, y, width, height);
}

// Функция проверки ошибок
inline ErrorCode GetError() {
    return static_cast<ErrorCode>(glGetError());
}

// Функции синхронизации
inline SyncHandle FenceSync(SyncCondition condition, SyncFlushFlag flags) {
    SyncHandle sync;
    sync.handle = glFenceSync(static_cast<GLenum>(condition), static_cast<GLbitfield>(flags));
    return sync;
}

inline SyncStatus ClientWaitSync(SyncHandle sync, SyncFlushFlag flags, GLuint64 timeout) {
    return static_cast<SyncStatus>(glClientWaitSync(sync.handle, static_cast<GLbitfield>(flags), timeout));
}

inline void WaitSync(SyncHandle sync, SyncFlushFlag flags, GLuint64 timeout) {
    glWaitSync(sync.handle, static_cast<GLbitfield>(flags), timeout);
}

inline void DeleteSync(SyncHandle sync) {
    if (sync.handle) {
        glDeleteSync(sync.handle);
    }
}

} // namespace gl46