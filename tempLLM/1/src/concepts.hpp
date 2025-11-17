#pragma once

#if __cplusplus >= 202002L
#include "opengl_types.hpp"
#include "handles.hpp"
#include <concepts>

namespace gl46::concepts {

// Концепты для типов хендлов
template<typename T>
concept BufferHandleType = std::same_as<T, BufferHandle>;

template<typename T>
concept TextureHandleType = std::same_as<T, TextureHandle>;

template<typename T>
concept ShaderHandleType = std::same_as<T, ShaderHandle>;

template<typename T>
concept ProgramHandleType = std::same_as<T, ProgramHandle>;

template<typename T>
concept VertexArrayHandleType = std::same_as<T, VertexArrayHandle>;

template<typename T>
concept FramebufferHandleType = std::same_as<T, FramebufferHandle>;

template<typename T>
concept RenderbufferHandleType = std::same_as<T, RenderbufferHandle>;

template<typename T>
concept SamplerHandleType = std::same_as<T, SamplerHandle>;

template<typename T>
concept TransformFeedbackHandleType = std::same_as<T, TransformFeedbackHandle>;

template<typename T>
concept QueryHandleType = std::same_as<T, QueryHandle>;

template<typename T>
concept PipelineHandleType = std::same_as<T, PipelineHandle>;

// Концепты для перечислений OpenGL
template<typename T>
concept ShaderTypeValue = std::same_as<T, ShaderType>;

template<typename T>
concept BufferTypeValue = std::same_as<T, BufferType>;

template<typename T>
concept BufferUsageValue = std::same_as<T, BufferUsage>;

template<typename T>
concept TextureTargetValue = std::same_as<T, TextureTarget>;

template<typename T>
concept TextureInternalFormatValue = std::same_as<T, TextureInternalFormat>;

template<typename T>
concept TextureFormatValue = std::same_as<T, TextureFormat>;

template<typename T>
concept TextureDataTypeValue = std::same_as<T, TextureDataType>;

template<typename T>
concept PrimitiveTypeValue = std::same_as<T, PrimitiveType>;

template<typename T>
concept ComparisonFuncValue = std::same_as<T, ComparisonFunc>;

template<typename T>
concept CullFaceModeValue = std::same_as<T, CullFaceMode>;

template<typename T>
concept FrontFaceModeValue = std::same_as<T, FrontFaceMode>;

template<typename T>
concept PolygonModeValue = std::same_as<T, PolygonMode>;

template<typename T>
concept BlendEquationValue = std::same_as<T, BlendEquation>;

template<typename T>
concept BlendFactorValue = std::same_as<T, BlendFactor>;

template<typename T>
concept DataTypeValue = std::same_as<T, DataType>;

template<typename T>
concept ClearBufferMaskValue = std::same_as<T, ClearBufferMask>;

template<typename T>
concept FramebufferTargetValue = std::same_as<T, FramebufferTarget>;

template<typename T>
concept FramebufferAttachmentValue = std::same_as<T, FramebufferAttachment>;

template<typename T>
concept FramebufferStatusValue = std::same_as<T, FramebufferStatus>;

template<typename T>
concept DrawBufferModeValue = std::same_as<T, DrawBufferMode>;

template<typename T>
concept EnableCapValue = std::same_as<T, EnableCap>;

template<typename T>
concept ErrorCodeValue = std::same_as<T, ErrorCode>;

template<typename T>
concept SyncConditionValue = std::same_as<T, SyncCondition>;

template<typename T>
concept SyncStatusValue = std::same_as<T, SyncStatus>;

template<typename T>
concept SyncFlushFlagValue = std::same_as<T, SyncFlushFlag>;

template<typename T>
concept VertexAttribPropertyValue = std::same_as<T, VertexAttribProperty>;

// Комбинированные концепты для более сложных проверок
template<typename T>
concept ValidHandleType = 
    BufferHandleType<T> || 
    TextureHandleType<T> || 
    ShaderHandleType<T> || 
    ProgramHandleType<T> ||
    VertexArrayHandleType<T> ||
    FramebufferHandleType<T> ||
    RenderbufferHandleType<T> ||
    SamplerHandleType<T> ||
    TransformFeedbackHandleType<T> ||
    QueryHandleType<T> ||
    PipelineHandleType<T>;

template<typename T>
concept ValidBufferType = 
    BufferTypeValue<T> || 
    BufferUsageValue<T>;

template<typename T>
concept ValidTextureType = 
    TextureTargetValue<T> || 
    TextureInternalFormatValue<T> || 
    TextureFormatValue<T> || 
    TextureDataTypeValue<T>;

template<typename T>
concept ValidShaderType = 
    ShaderTypeValue<T>;

template<typename T>
concept ValidRenderType = 
    PrimitiveTypeValue<T> || 
    CullFaceModeValue<T> || 
    FrontFaceModeValue<T> || 
    PolygonModeValue<T> ||
    BlendEquationValue<T> ||
    BlendFactorValue<T>;

template<typename T>
concept ValidResourceType = 
    ValidHandleType<T> || 
    ValidBufferType<T> || 
    ValidTextureType<T> || 
    ValidShaderType<T> || 
    ValidRenderType<T>;

// Концепт для проверки, что тип является арифметическим (для данных буфера)
template<typename T>
concept ArithmeticType = std::is_arithmetic_v<T>;

// Концепт для проверки, что тип является массивом арифметических значений
template<typename T>
concept ArithmeticArray = requires(T& arr) {
    std::data(arr);
    std::size(arr);
    requires ArithmeticType<std::remove_reference_t<decltype(std::declval<T>()[0])>>;
};

// Концепт для проверки, что тип может быть использован в шейдерах
template<typename T>
concept ShaderCompatibleType = 
    ArithmeticType<T> || 
    ArithmeticArray<T> ||
    std::is_same_v<T, std::vector<float>> ||
    std::is_same_v<T, std::vector<int>> ||
    std::is_same_v<T, std::vector<unsigned int>>;

} // namespace gl46::concepts

#endif // __cplusplus >= 202002L