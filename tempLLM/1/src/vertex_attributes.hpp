#pragma once

#include "opengl_types.hpp"
#include "handles.hpp"
#include "functions.hpp"
#include "concepts.hpp"

#if __cplusplus >= 202002L
#include <concepts>
#endif

namespace gl46 {

// Безопасные шейдерные атрибуты с использованием шаблонов и концептов
template<DataType data_type, int size, bool normalized = false>
struct VertexAttribute {
    static constexpr DataType Type = data_type;
    static constexpr int Size = size;
    static constexpr bool Normalized = normalized;
    
    GLuint index;
    GLuint relative_offset;
    
    constexpr VertexAttribute(GLuint idx, GLuint offset) : index(idx), relative_offset(offset) {}
    
    void ApplyTo(VertexArrayHandle vao) const {
#if __cplusplus >= 202002L
        static_assert(concepts::DataTypeValue<DataType>, "DataType must be a valid OpenGL DataType");
        static_assert(Size >= 1 && Size <= 4, "Size must be between 1 and 4");
#endif
        
        if constexpr (std::is_same_v<decltype(Type), decltype(DataType::Float)> || 
                      std::is_same_v<decltype(Type), decltype(DataType::Double)> ||
                      std::is_same_v<decltype(Type), decltype(DataType::HalfFloat)>) {
            // Для float-типов используем обычный формат
            VertexArrayAttribFormat(vao, VertexAttribBinding{index}, Size, Type, Normalized, relative_offset);
        } else {
            // Для целочисленных типов используем целочисленный формат
            VertexArrayAttribIFormat(vao, VertexAttribBinding{index}, Size, Type, relative_offset);
        }
        
        EnableVertexArrayAttrib(vao, VertexAttribBinding{index});
    }
};

// Специализированные типы для часто используемых атрибутов
using PositionAttribute = VertexAttribute<DataType::Float, 3>;
using NormalAttribute = VertexAttribute<DataType::Float, 3>;
using TexCoordAttribute = VertexAttribute<DataType::Float, 2>;
using ColorAttribute = VertexAttribute<DataType::Float, 4, true>;
using TangentAttribute = VertexAttribute<DataType::Float, 3>;
using BitangentAttribute = VertexAttribute<DataType::Float, 3>;

// Вспомогательная структура для описания вершинного формата
template<typename... Attributes>
struct VertexFormat {
    std::tuple<Attributes...> attributes;
    
    constexpr VertexFormat(Attributes... attrs) : attributes(attrs...) {}
    
    void ApplyTo(VertexArrayHandle vao) const {
        std::apply([vao](const auto&... attrs) {
            (attrs.ApplyTo(vao), ...);
        }, attributes);
    }
    
    constexpr size_t GetStride() const {
        return GetStrideImpl(std::index_sequence_for<Attributes...>{});
    }
    
private:
    template<std::size_t... Is>
    constexpr size_t GetStrideImpl(std::index_sequence<Is...>) const {
        return (GetAttributeSize(std::get<Is>(attributes)) + ... + 0);
    }
    
    template<typename Attr>
    constexpr static size_t GetAttributeSize(const Attr& attr) {
        return attr.Size * GetDataTypeSize(attr.Type);
    }
    
    constexpr static size_t GetDataTypeSize(DataType type) {
        switch (type) {
            case DataType::Byte:
            case DataType::UnsignedByte:
                return sizeof(GLbyte);
            case DataType::Short:
            case DataType::UnsignedShort:
                return sizeof(GLshort);
            case DataType::Int:
            case DataType::UnsignedInt:
                return sizeof(GLint);
            case DataType::Float:
                return sizeof(GLfloat);
            case DataType::Double:
                return sizeof(GLdouble);
            case DataType::HalfFloat:
                return sizeof(GLhalf);
            default:
                return sizeof(GLfloat); // Значение по умолчанию
        }
    }
};

// Вспомогательные функции для создания атрибутов
template<DataType T, int S, bool N = false>
constexpr VertexAttribute<T, S, N> MakeVertexAttribute(GLuint index, GLuint offset) {
    return VertexAttribute<T, S, N>{index, offset};
}

// Макросы для удобного создания часто используемых атрибутов
#define GL46_POSITION_ATTR(index, offset) MakeVertexAttribute<DataType::Float, 3>(index, offset)
#define GL46_NORMAL_ATTR(index, offset) MakeVertexAttribute<DataType::Float, 3>(index, offset)
#define GL46_TEXCOORD_ATTR(index, offset) MakeVertexAttribute<DataType::Float, 2>(index, offset)
#define GL46_COLOR_ATTR(index, offset) MakeVertexAttribute<DataType::Float, 4, true>(index, offset)

} // namespace gl46