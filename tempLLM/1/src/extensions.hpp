#pragma once

#include "opengl_types.hpp"
#include "handles.hpp"
#include "functions.hpp"

// Заголовочный файл для поддержки расширений OpenGL 4.6
namespace gl46 {

// ARB_sparse_texture
#ifdef GL_ARB_sparse_texture
inline void TexPageCommitmentARB(TextureTarget target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLboolean commit) {
    glTexPageCommitmentARB(static_cast<GLenum>(target), level, xoffset, yoffset, zoffset, width, height, depth, commit);
}
#endif

// ARB_sparse_buffer
#ifdef GL_ARB_sparse_buffer
inline void BufferPageCommitmentARB(BufferTarget target, GLintptr offset, GLsizeiptr size, GLboolean commit) {
    glBufferPageCommitmentARB(static_cast<GLenum>(target), offset, size, commit);
}
#endif

// ARB_parallel_shader_compile
#ifdef GL_ARB_parallel_shader_compile
inline void MaxShaderCompilerThreadsARB(GLuint count) {
    glMaxShaderCompilerThreadsARB(count);
}
#endif

// ARB_pipeline_statistics_query
// Перечисления уже включены в opengl_types.hpp

// ARB_transform_feedback_overflow_query
// Перечисления уже включены в opengl_types.hpp

// EXT_texture_compression_s3tc
// Поддержка форматов уже включена в opengl_types.hpp

// ARB_bindless_texture
#ifdef GL_ARB_bindless_texture
inline GLuint64 GetTextureHandleARB(TextureHandle texture) {
    return glGetTextureHandleARB(texture.handle);
}

inline GLuint64 GetTextureSamplerHandleARB(TextureHandle texture, SamplerHandle sampler) {
    return glGetTextureSamplerHandleARB(texture.handle, sampler.handle);
}

inline void MakeTextureHandleResidentARB(GLuint64 handle) {
    glMakeTextureHandleResidentARB(handle);
}

inline void MakeTextureHandleNonResidentARB(GLuint64 handle) {
    glMakeTextureHandleNonResidentARB(handle);
}

inline GLuint64 GetImageHandleARB(TextureHandle texture, GLint level, GLboolean layered, GLint layer, TextureFormat format) {
    return glGetImageHandleARB(texture.handle, level, layered, layer, static_cast<GLenum>(format));
}

inline void MakeImageHandleResidentARB(GLuint64 handle, AccessMode access) {
    glMakeImageHandleResidentARB(handle, static_cast<GLenum>(access));
}

inline void MakeImageHandleNonResidentARB(GLuint64 handle) {
    glMakeImageHandleNonResidentARB(handle);
}
#endif

// ARB_shader_draw_parameters
// Поддержка встроена в OpenGL 4.6

// ARB_texture_filter_anisotropic
// Поддержка включена в core функции

// ARB_compute_variable_group_size
#ifdef GL_ARB_compute_variable_group_size
inline void DispatchComputeGroupSizeARB(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z, GLuint group_size_x, GLuint group_size_y, GLuint group_size_z) {
    glDispatchComputeGroupSizeARB(num_groups_x, num_groups_y, num_groups_z, group_size_x, group_size_y, group_size_z);
}
#endif

// ARB_indirect_parameters
#ifdef GL_ARB_indirect_parameters
inline void MultiDrawArraysIndirectCountARB(PrimitiveType mode, const void *indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride) {
    glMultiDrawArraysIndirectCountARB(static_cast<GLenum>(mode), indirect, drawcount, maxdrawcount, stride);
}

inline void MultiDrawElementsIndirectCountARB(PrimitiveType mode, DrawElementsType type, const void *indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride) {
    glMultiDrawElementsIndirectCountARB(static_cast<GLenum>(mode), static_cast<GLenum>(type), indirect, drawcount, maxdrawcount, stride);
}
#endif

// ARB_shader_clock
#ifdef GL_ARB_shader_clock
inline GLuint64 GetShaderClockARB(ShaderHandle shader) {
    return glGetShaderClockARB(shader.handle);
}
#endif

// ARB_polygon_offset_clamp
#ifdef GL_ARB_polygon_offset_clamp
inline void PolygonOffsetClampARB(GLfloat factor, GLfloat units, GLfloat clamp) {
    glPolygonOffsetClampARB(factor, units, clamp);
}
#endif

// ARB_shader_viewport_layer_array
// Поддержка встроена в OpenGL 4.1+

// ARB_uniform_buffer_object
// Поддержка встроена в OpenGL 3.1+

// ARB_shader_storage_buffer_object
// Поддержка встроена в OpenGL 4.3+

// ARB_texture_storage
// Поддержка встроена в OpenGL 4.2+

// ARB_texture_storage_multisample
// Поддержка встроена в OpenGL 4.3+

// ARB_buffer_storage
// Поддержка встроена в OpenGL 4.4+

// ARB_clear_texture
// Поддержка встроена в OpenGL 4.4+

// ARB_copy_image
// Поддержка встроена в OpenGL 4.3+

// ARB_texture_view
// Поддержка встроена в OpenGL 4.3+

// ARB_vertex_attrib_binding
// Поддержка встроена в OpenGL 4.3+

// ARB_direct_state_access
// Поддержка встроена в OpenGL 4.5+

// ARB_separate_shader_objects
// Поддержка встроена в OpenGL 4.1+

// ARB_compute_shader
// Поддержка встроена в OpenGL 4.3+

// ARB_tessellation_shader
// Поддержка встроена в OpenGL 4.0+

// ARB_geometry_shader4
// Поддержка встроена в OpenGL 3.2+

// ARB_enhanced_layouts
// Поддержка встроена в OpenGL 4.4+

// ARB_multi_bind
// Поддержка встроена в OpenGL 4.4+

// ARB_map_buffer_alignment
// Поддержка встроена в OpenGL 3.2+

// ARB_shader_atomic_counters
// Поддержка встроена в OpenGL 4.2+

// ARB_shader_image_load_store
// Поддержка встроена в OpenGL 4.2+

// ARB_shader_image_size
// Поддержка встроена в OpenGL 4.3+

// ARB_shader_ballot
// Поддержка через compute shaders

// ARB_shader_group_vote
// Поддержка через compute shaders

// ARB_shader_texture_image_samples
// Поддержка встроена в OpenGL 4.5+

// ARB_gl_spirv
// Поддержка SPIR-V через ARB_spirv_extensions

// ARB_spirv_extensions
// Поддержка SPIR-V

// ARB_shader_atomic_counter_ops
// Поддержка встроена в OpenGL 4.2+

// ARB_shader_ballot
// Поддержка через compute shaders

// ARB_shader_clock
// Поддержка через GL_ARB_shader_clock

// ARB_shader_viewport_index_layer
// Поддержка встроена в OpenGL 4.1+

// ARB_sparse_texture2
// Дополнительные возможности для sparse текстур

// ARB_sparse_texture_clamp
// Дополнительные возможности для sparse текстур

// ARB_texture_filter_minmax
// Поддержка встроена в OpenGL 4.5+

// ARB_sample_locations
#ifdef GL_ARB_sample_locations
inline void SampleLocationsfvARB(SampleHandle sample, GLuint startSubsample, GLsizei count, const GLfloat* v) {
    glSampleLocationsfvARB(static_cast<GLenum>(sample), startSubsample, count, v);
}

inline void EvaluateDepthValuesARB() {
    glEvaluateDepthValuesARB();
}
#endif

// ARB_fragment_shader_interlock
// Поддержка через fragment shader extensions

// ARB_shader_group_vote
// Поддержка через compute shaders

// ARB_texture_barrier
#ifdef GL_ARB_texture_barrier
inline void TextureBarrier() {
    glTextureBarrier();
}
#endif

// ARB_cull_distance
// Поддержка встроена в OpenGL 4.5+

// ARB_conditional_render_inverted
// Поддержка встроена в OpenGL 4.5+

// ARB_derivative_control
// Поддержка встроена в OpenGL 4.5+

// ARB_shader_texture_image_samples
// Поддержка встроена в OpenGL 4.5+

// ARB_transform_feedback2
// Поддержка встроена в OpenGL 4.0+

// ARB_transform_feedback3
// Поддержка встроена в OpenGL 4.0+

// ARB_ES3_2_compatibility
// Поддержка ES 3.2 compatibility

// ARB_fragment_shader_interlock
// Поддержка через fragment shader extensions

// ARB_gpu_shader_int64
// Поддержка 64-битных целых в шейдерах

// ARB_shader_ballot
// Поддержка через compute shaders

// ARB_shader_clock
// Поддержка через GL_ARB_shader_clock

// ARB_shader_viewport_index_layer
// Поддержка встроена в OpenGL 4.1+

// ARB_sparse_texture2
// Дополнительные возможности для sparse текстур

// ARB_sparse_texture_clamp
// Дополнительные возможности для sparse текстур

// ARB_texture_filter_minmax
// Поддержка встроена в OpenGL 4.5+

// ARB_pipeline_statistics_query
// Поддержка встроена в OpenGL 4.6

// ARB_transform_feedback_overflow_query
// Поддержка встроена в OpenGL 4.6

} // namespace gl46