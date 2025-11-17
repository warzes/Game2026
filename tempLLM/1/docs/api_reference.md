# API Reference для OpenGL 4.6 Type-Safe Wrapper

## Оглавление
1. [Типы данных](#типы-данных)
2. [Хендлы ресурсов](#хендлы-ресурсов)
3. [Глобальные функции](#глобальные-функции)
4. [RAII обертки](#raii-обертки)
5. [Концепты](#концепты)
6. [Пакетные операции](#пакетные-операции)
7. [Вершинные атрибуты](#вершинные-атрибуты)
8. [Система валидации](#система-валидации)
9. [Исключения](#исключения)

## Типы данных

Все OpenGL перечисления заменены типобезопасными `enum class`:

### ComparisonFunc
```cpp
enum class ComparisonFunc : uint8_t {
    Never,      // GL_NEVER
    Less,       // GL_LESS
    Equal,      // GL_EQUAL
    LessEqual,  // GL_LEQUAL
    Greater,    // GL_GREATER
    NotEqual,   // GL_NOTEQUAL
    GreaterEqual, // GL_GEQUAL
    Always      // GL_ALWAYS
};
```

### BufferType
```cpp
enum class BufferType : GLenum {
    ArrayBuffer,           // GL_ARRAY_BUFFER
    ElementArrayBuffer,    // GL_ELEMENT_ARRAY_BUFFER
    UniformBuffer,         // GL_UNIFORM_BUFFER
    ShaderStorageBuffer,   // GL_SHADER_STORAGE_BUFFER
    // ... и другие типы буферов
};
```

### ShaderType
```cpp
enum class ShaderType : GLenum {
    VertexShader,        // GL_VERTEX_SHADER
    FragmentShader,      // GL_FRAGMENT_SHADER
    GeometryShader,      // GL_GEOMETRY_SHADER
    TessControlShader,   // GL_TESS_CONTROL_SHADER
    TessEvaluationShader, // GL_TESS_EVALUATION_SHADER
    ComputeShader        // GL_COMPUTE_SHADER
};
```

## Хендлы ресурсов

Хендлы - это легковесные структуры, содержащие только `GLuint handle`:

```cpp
struct BufferHandle final {
    GLuint handle{ 0u };
    
    explicit constexpr BufferHandle(GLuint h = 0u) noexcept : handle(h) {}
    constexpr bool operator==(const BufferHandle& other) const noexcept { return handle == other.handle; }
    constexpr bool operator!=(const BufferHandle& other) const noexcept { return handle != other.handle; }
    constexpr bool operator<(const BufferHandle& other) const noexcept { return handle < other.handle; }
    constexpr bool valid() const noexcept { return handle != 0u; }
};
```

Аналогично определены `TextureHandle`, `ShaderHandle`, `ProgramHandle`, `VertexArrayHandle`, `FramebufferHandle`, `RenderbufferHandle`, `SamplerHandle`, `TransformFeedbackHandle`, `QueryHandle`, `PipelineHandle`.

## Глобальные функции

Все функции используют типобезопасные перечисления:

```cpp
// Создание буфера
BufferHandle CreateBuffer(BufferType target, size_t size, const void* data, BufferUsage usage);

// Установка данных буфера
void SetBufferData(BufferHandle buffer, size_t size, const void* data, BufferUsage usage);

// Создание текстуры
TextureHandle CreateTexture(TextureTarget target);

// Установка изображения текстуры
void SetTextureImage2D(
    TextureHandle texture,
    GLint level,
    TextureInternalFormat internalFormat,
    GLsizei width,
    GLsizei height,
    GLint border,
    TextureFormat format,
    TextureDataType type,
    const void* data
);

// Компиляция шейдера
void CompileShader(ShaderHandle shader);

// Линковка программы
void LinkProgram(ProgramHandle program);

// Рендеринг
void DrawArrays(PrimitiveType mode, GLint first, GLsizei count);
void DrawElements(PrimitiveType mode, GLsizei count, DataType type, const void* indices);
```

## RAII обертки

RAII обертки обеспечивают автоматическое управление ресурсами:

```cpp
class Buffer {
public:
    Buffer(BufferType target, size_t size, const void* data = nullptr, BufferUsage usage = BufferUsage::StaticDraw);
    ~Buffer();
    
    // Запрет копирования
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    
    // Разрешение перемещения
    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;
    
    BufferHandle GetHandle() const;
    void SetData(size_t size, const void* data, BufferUsage usage = BufferUsage::StaticDraw);
    // ... другие методы
};
```

## Концепты

Концепты обеспечивают типобезопасность на этапе компиляции:

```cpp
#if __cplusplus >= 202002L
namespace gl46::concepts {

template<typename T>
concept BufferHandleType = std::same_as<T, BufferHandle>;

template<typename T>
concept ValidResourceType = 
    ValidHandleType<T> || 
    ValidBufferType<T> || 
    ValidTextureType<T> || 
    ValidShaderType<T> || 
    ValidRenderType<T>;

} // namespace gl46::concepts
#endif
```

## Пакетные операции

Классы для оптимизации производительности:

```cpp
// Пакетная загрузка данных в буфер
template<typename T>
class BatchBuffer {
public:
    void Add(const T& element);
    template<size_t N> void AddArray(const T (&data)[N]);
    void AddVector(const std::vector<T>& data);
    void FillMapped(const std::vector<T>& data);
    // ... другие методы
};

// Пакетная отрисовка
class BatchRenderer {
public:
    void AddDrawCommand(GLint first, GLsizei count);
    void ExecuteDrawCommands(PrimitiveType mode);
    void Clear();
};
```

## Вершинные атрибуты

Типобезопасные вершинные атрибуты:

```cpp
// Шаблонный атрибут
template<DataType data_type, int size, bool normalized = false>
struct VertexAttribute {
    static constexpr DataType Type = data_type;
    static constexpr int Size = size;
    static constexpr bool Normalized = normalized;
    
    GLuint index;
    GLuint relative_offset;
    
    void ApplyTo(VertexArrayHandle vao) const;
};

// Использование
auto position_attr = VertexAttribute<DataType::Float, 3>{0, 0};
auto color_attr = VertexAttribute<DataType::Float, 4, true>{1, 3 * sizeof(float)};

// Вспомогательные типы
using PositionAttribute = VertexAttribute<DataType::Float, 3>;
using ColorAttribute = VertexAttribute<DataType::Float, 4, true>;
```

## Система валидации

Класс для проверки ошибок OpenGL:

```cpp
namespace gl46::validation {

class Validator {
public:
    static void CheckError(const char* operation);
    template<typename HandleType> static void ValidateHandle(const HandleType& handle, const char* resource_type);
    static void ValidateShaderCompilation(ShaderHandle shader);
    static void ValidateProgramLink(ProgramHandle program);
    static FramebufferStatus ValidateFramebufferStatus(FramebufferHandle framebuffer, FramebufferTarget target);
};

} // namespace gl46::validation
```

## Исключения

Иерархия исключений для безопасной обработки ошибок:

```cpp
namespace gl46::exceptions {

class OpenGLException : public std::exception {
public:
    const char* what() const noexcept override;
    ErrorCode GetErrorCode() const noexcept;
};

class BufferCreationException : public OpenGLException { /* ... */ };
class ShaderCompilationException : public OpenGLException { /* ... */ };
class ProgramLinkException : public OpenGLException { /* ... */ };
class TextureCreationException : public OpenGLException { /* ... */ };
class FramebufferException : public OpenGLException { /* ... */ };
class InvalidHandleException : public OpenGLException { /* ... */ };

} // namespace gl46::exceptions