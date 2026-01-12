#include "stdafx.h"
#include "NanoOpenGL3.h"
#include "NanoLog.h"
#include "NanoCore.h"
//=============================================================================
std::unordered_map<SamplerStateInfo, SamplerHandle> SamplerCache;
//=============================================================================
GLenum GetColorFormatGL(ColorFormat format) noexcept
{
	switch (format)
	{
	case ColorFormat::Red:  return GL_RED;
	case ColorFormat::RG:   return GL_RG;
	case ColorFormat::RGB:  return GL_RGB;
	case ColorFormat::RGBA: return GL_RGBA;
	default: std::unreachable();
	}
}
//=============================================================================
GLenum GetDataTypeGL(DataType dataType) noexcept
{
	return (dataType == DataType::Float) ? GL_FLOAT : GL_UNSIGNED_BYTE;
}
//=============================================================================
[[nodiscard]] inline GLenum GetGLEnum(ComparisonFunc func) noexcept
{
	switch (func) {
	case ComparisonFunc::Never:        return GL_NEVER;
	case ComparisonFunc::Less:         return GL_LESS;
	case ComparisonFunc::Equal:        return GL_EQUAL;
	case ComparisonFunc::LessEqual:    return GL_LEQUAL;
	case ComparisonFunc::Greater:      return GL_GREATER;
	case ComparisonFunc::NotEqual:     return GL_NOTEQUAL;
	case ComparisonFunc::GreaterEqual: return GL_GEQUAL;
	case ComparisonFunc::Always:       return GL_ALWAYS;
	default: std::unreachable();
	}
}
//=============================================================================
[[nodiscard]] inline GLenum GetGLEnum(BlendFactor factor) noexcept
{
	switch (factor) {
	case BlendFactor::Zero:             return GL_ZERO;
	case BlendFactor::One:              return GL_ONE;
	case BlendFactor::SrcColor:         return GL_SRC_COLOR;
	case BlendFactor::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
	case BlendFactor::DstColor:         return GL_DST_COLOR;
	case BlendFactor::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
	case BlendFactor::SrcAlpha:         return GL_SRC_ALPHA;
	case BlendFactor::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
	case BlendFactor::DstAlpha:         return GL_DST_ALPHA;
	case BlendFactor::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
	default: std::unreachable();
	}
}
//=============================================================================
[[nodiscard]] inline GLenum GetGLEnum(CullFace cull) noexcept
{
	switch (cull) {
	case CullFace::Front:        return GL_FRONT;
	case CullFace::Back:         return GL_BACK;
	case CullFace::FrontAndBack: return GL_FRONT_AND_BACK;
	default: std::unreachable();
	}
}
//=============================================================================
[[nodiscard]] inline GLenum GetGLEnum(BufferTarget type) noexcept
{
	switch (type) {
	case BufferTarget::Array:        return GL_ARRAY_BUFFER;
	case BufferTarget::ElementArray: return GL_ELEMENT_ARRAY_BUFFER;
	case BufferTarget::Uniform:      return GL_UNIFORM_BUFFER;
	default: std::unreachable();
	}
}
//=============================================================================
[[nodiscard]] inline GLenum GetGLEnum(PolygonMode mode) noexcept
{
	switch (mode) {
	case PolygonMode::Point: return GL_POINT;
	case PolygonMode::Line:  return GL_LINE;
	case PolygonMode::Fill:  return GL_FILL;
	default: std::unreachable();
	}
}
//=============================================================================
[[nodiscard]] inline GLenum GetGLEnum(BufferUsage mode) noexcept
{
	switch (mode) {
	case BufferUsage::StaticDraw: return GL_STATIC_DRAW;
	case BufferUsage::DynamicDraw: return GL_DYNAMIC_DRAW;
	case BufferUsage::StreamDraw: return GL_STREAM_DRAW;
	case BufferUsage::StaticRead: return GL_STATIC_READ;
	case BufferUsage::DynamicRead: return GL_DYNAMIC_READ;
	case BufferUsage::StreamRead: return GL_STREAM_READ;
	case BufferUsage::StaticCopy: return GL_STATIC_COPY;
	case BufferUsage::DynamicCopy: return GL_DYNAMIC_COPY;
	case BufferUsage::StreamCopy: return GL_STREAM_COPY;
	default: std::unreachable();
	}
}
//=============================================================================
[[nodiscard]] inline GLint GetGLEnum(TextureFilter filter) noexcept
{
	switch (filter) {
	case TextureFilter::Nearest:              return GL_NEAREST;
	case TextureFilter::Linear:               return GL_LINEAR;
	case TextureFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
	case TextureFilter::LinearMipmapNearest:  return GL_LINEAR_MIPMAP_NEAREST;
	case TextureFilter::NearestMipmapLinear:  return GL_NEAREST_MIPMAP_LINEAR;
	case TextureFilter::LinearMipmapLinear:   return GL_LINEAR_MIPMAP_LINEAR;
	default: std::unreachable();
	}
}
//=============================================================================
[[nodiscard]] inline GLint GetGLEnum(TextureWrap wrap) noexcept
{
	switch (wrap) {
	case TextureWrap::Repeat:         return GL_REPEAT;
	case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
	case TextureWrap::ClampToEdge:    return GL_CLAMP_TO_EDGE;
	case TextureWrap::ClampToBorder:  return GL_CLAMP_TO_BORDER;
	default: std::unreachable();
	}
}
//=============================================================================
std::string preprocessShaderCode(const std::string& line, const std::string& directory, unsigned int level)
{
	static const std::regex re("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
	std::smatch matches;

	if (regex_search(line, matches, re))
	{
		std::string path = matches[1].str();
		std::string loadShaderCode(const std::string&, unsigned int);
		return loadShaderCode(directory + "/" + path, level);
	}
	else
	{
		return line;
	}
}
//=============================================================================
std::string loadShaderCode(const std::string& path, unsigned int level)
{
	Debug("Load Shader file: " + path);

	if (level > 32)
	{
		Error("Header inclusion depth limit reached, might be caused by cyclic header inclusion");
		return {};
	}

	std::stringstream shaderStream;
	std::string directory = path.substr(0, path.find_last_of('/'));

	std::ifstream shaderFile(path);
	if (!shaderFile.is_open())
	{
		Error("Fail to open file: " + path);
		return {};
	}

	std::string line;
	while (std::getline(shaderFile, line))
	{
		shaderStream << preprocessShaderCode(line, directory, level + 1) << std::endl;
	}

	return shaderStream.str();
}
//=============================================================================
std::string LoadShaderCode(const std::string& path, const std::vector<std::string>& defines)
{
	Debug("Load Shader file: " + path);

	std::stringstream shaderStream;
	std::string directory = path.substr(0, path.find_last_of('/'));

	std::ifstream shaderFile(path);
	if (!shaderFile.is_open())
	{
		Error("Fail to open file: " + path);
		return {};
	}

	std::string line;
	unsigned int lineNumber = 0;
	while (std::getline(shaderFile, line))
	{
		if (lineNumber == 1)
		{
			for (auto itr = defines.begin(); itr != defines.end(); itr++)
			{
				shaderStream << "#define " << *itr << std::endl;
			}
		}

		shaderStream << preprocessShaderCode(line, directory, 1) << std::endl;
		lineNumber++;
	}

	return shaderStream.str();
}
//=============================================================================
[[nodiscard]] inline std::string shaderStageToString(GLenum stage)
{
	switch (stage)
	{
	case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
	case GL_GEOMETRY_SHADER: return "GL_GEOMETRY_SHADER";
	case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
	default: std::unreachable();
	}
}
//=============================================================================
[[nodiscard]] inline std::string printShaderSource(const char* text)
{
	if (!text) return "";

	std::ostringstream oss;
	int line = 1;
	oss << "\n(" << std::setw(3) << std::setfill(' ') << line << "): ";

	while (*text)
	{
		if (*text == '\n')
		{
			oss << '\n';
			line++;
			oss << "(" << std::setw(3) << std::setfill(' ') << line << "): ";
		}
		else if (*text != '\r')
		{
			oss << *text;
		}
		text++;
	}
	return oss.str();
}
//=============================================================================
[[nodiscard]] inline GLuint compileShaderGLSL(GLenum stage, std::string_view sourceGLSL)
{
	if (sourceGLSL.empty())
	{
		Error("Failed to create OpenGL shader object for stage: " + std::string(shaderStageToString(stage)) + ". Source code Empty.");
		return { 0 };
	}

	GLuint shader = glCreateShader(stage);
	if (!shader)
	{
		Error("Failed to create OpenGL shader object for stage: " + std::string(shaderStageToString(stage)));
		return { 0 };
	}
	const GLchar* strings = sourceGLSL.data();
	glShaderSource(shader, 1, &strings, nullptr);
	glCompileShader(shader);

	GLint success{ 0 };
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint infoLength{ 0 };
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);
		std::string infoLog;
		if (infoLength > 1)
		{
			infoLog.resize(static_cast<size_t>(infoLength - 1)); // исключаем \0
			glGetShaderInfoLog(shader, infoLength, nullptr, infoLog.data());
		}
		else
		{
			infoLog = "<no info log>";
		}

		std::string logError = "OPENGL " + shaderStageToString(stage) + ": Shader compilation failed: " + infoLog;
		if (strings != nullptr) logError += ", Source: \n" + printShaderSource(strings);
		Error(logError);
		return 0;
	}

	return shader;
}
//=============================================================================
ProgramHandle CreateShaderProgram(std::string_view vertexShader)
{
	return CreateShaderProgram(vertexShader, "", "");
}
//=============================================================================
ProgramHandle CreateShaderProgram(std::string_view vertexShader, std::string_view fragmentShader)
{
	return CreateShaderProgram(vertexShader, "", fragmentShader);
}
//=============================================================================
ProgramHandle CreateShaderProgram(std::string_view vertexShader, std::string_view geometryShader, std::string_view fragmentShader)
{
	struct LocalShader final
	{
		~LocalShader() { if (id) glDeleteShader(id); }
		GLuint id{ 0 };
	};

	LocalShader vs;
	if (!vertexShader.empty())
	{
		vs.id = compileShaderGLSL(GL_VERTEX_SHADER, vertexShader);
		if (!vs.id) return {};
	}
	LocalShader gs;
	if (!geometryShader.empty())
	{
		gs.id = compileShaderGLSL(GL_GEOMETRY_SHADER, geometryShader);
		if (!gs.id) return {};
	}
	LocalShader fs;
	if (!fragmentShader.empty())
	{
		fs.id = compileShaderGLSL(GL_FRAGMENT_SHADER, fragmentShader);
		if (!fs.id) return {};
	}
	if (vs.id == 0 && gs.id == 0 && fs.id == 0)
	{
		Error("Shader not valid");
		return {};
	}

	ProgramHandle program(glCreateProgram());
	assert(program.handle);

	if (vs.id) glAttachShader(program.handle, vs.id);
	if (gs.id) glAttachShader(program.handle, gs.id);
	if (fs.id) glAttachShader(program.handle, fs.id);
	glLinkProgram(program.handle);

	GLint success{ 0 };
	glGetProgramiv(program.handle, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLint length = 512;
		glGetProgramiv(program.handle, GL_INFO_LOG_LENGTH, &length);
		std::string infoLog;
		infoLog.resize(static_cast<size_t>(length + 1), '\0');
		glGetProgramInfoLog(program.handle, length, nullptr, infoLog.data());
		glDeleteProgram(program.handle);
		Error("Failed to compile graphics pipeline.\n" + infoLog);
		program.handle = 0;
	}

	if (vs.id) glDetachShader(program.handle, vs.id);
	if (gs.id) glDetachShader(program.handle, gs.id);
	if (fs.id) glDetachShader(program.handle, fs.id);

	return program;
}
//=============================================================================
ProgramHandle LoadShaderProgram(const std::string& vsFile, const std::vector<std::string>& defines)
{
	return CreateShaderProgram(LoadShaderCode(vsFile, defines));
}
//=============================================================================
ProgramHandle LoadShaderProgram(const std::string& vsFile, const std::string& fsFile, const std::vector<std::string>& defines)
{
	return CreateShaderProgram(LoadShaderCode(vsFile, defines), LoadShaderCode(fsFile, defines));
}
//=============================================================================
ProgramHandle LoadShaderProgram(const std::string& vsFile, const std::string& gsFile, const std::string& fsFile, const std::vector<std::string>& defines)
{
	return CreateShaderProgram(LoadShaderCode(vsFile, defines), LoadShaderCode(gsFile, defines), LoadShaderCode(fsFile, defines));
}
//=============================================================================
int GetUniformLocation(ProgramHandle program, std::string_view name)
{
	return glGetUniformLocation(program.handle, name.data());
}
//=============================================================================
void SetUniform(int id, bool b)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	glUniform1i(id, b ? 1 : 0);
}
//=============================================================================
void SetUniform(int id, float s)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	glUniform1f(id, s);
}
//=============================================================================
void SetUniform(int id, int s)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	glUniform1i(id, s);
}
//=============================================================================
void SetUniform(int id, unsigned s)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	glUniform1ui(id, s);
}
//=============================================================================
void SetUniform(int id, const glm::vec2& v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	glUniform2fv(id, 1, glm::value_ptr(v));
}
//=============================================================================
void SetUniform(int id, std::span<const glm::vec2> v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	const GLsizei count = static_cast<GLsizei>(v.size());
	glUniform2fv(id, count, glm::value_ptr(v[0]));
}
//=============================================================================
void SetUniform(int id, const glm::vec3& v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	glUniform3fv(id, 1, glm::value_ptr(v));
}
//=============================================================================
void SetUniform(int id, std::span<const glm::vec3> v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	const GLsizei count = static_cast<GLsizei>(v.size());
	glUniform3fv(id, count, glm::value_ptr(v[0]));
}
//=============================================================================
void SetUniform(int id, const glm::vec4& v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	glUniform4fv(id, 1, glm::value_ptr(v));
}
//=============================================================================
void SetUniform(int id, std::span<const glm::vec4> v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	const GLsizei count = static_cast<GLsizei>(v.size());
	glUniform4fv(id, count, glm::value_ptr(v[0]));
}
//=============================================================================
void SetUniform(int id, const glm::quat& v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	glUniform4fv(id, 1, glm::value_ptr(v));
}
//=============================================================================
void SetUniform(int id, const glm::mat3& m)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(m));
}
//=============================================================================
void SetUniform(int id, const glm::mat4& m)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(m));
}
//=============================================================================
void SpecifyVertexAttributes(size_t vertexSize, std::span<const VertexAttribute> attributes)
{
	assert(vertexSize > 0);
	assert(attributes.size() > 0);

	for (size_t i = 0; i < attributes.size(); i++)
	{
		const auto& attr = attributes[i];
		const GLuint index = static_cast<GLuint>(i);

		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, attr.size, attr.type, attr.normalized ? GL_TRUE : GL_FALSE, static_cast<GLsizei>(vertexSize), attr.offset);
		glVertexAttribDivisor(index, attr.perInstance ? 1 : 0);
	}
}
//=============================================================================
void VertexP3::SetVertexAttributes()
{
	const size_t vertexSize = sizeof(VertexP3);
	const VertexAttribute attributes[] =
	{
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(VertexP3, position)},
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================
void VertexPNT::SetVertexAttributes()
{
	const size_t vertexSize = sizeof(VertexPNT);
	const VertexAttribute attributes[] =
	{
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(VertexPNT, position)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(VertexPNT, normal)},
		{.type = GL_FLOAT, .size = 2, .offset = (void*)offsetof(VertexPNT, texcoord)},
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================
void QuadVertex::SetVertexAttributes()
{
	const size_t vertexSize = sizeof(QuadVertex);
	const VertexAttribute attributes[] =
	{
		{.type = GL_FLOAT, .size = 2, .offset = (void*)offsetof(QuadVertex, position)},
		{.type = GL_FLOAT, .size = 2, .offset = (void*)offsetof(QuadVertex, texCoord)},
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================
void MeshVertex::SetVertexAttributes()
{
	const size_t vertexSize = sizeof(MeshVertex);
	const VertexAttribute attributes[] =
	{
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, position)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, color)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, normal)},
		{.type = GL_FLOAT, .size = 2, .offset = (void*)offsetof(MeshVertex, texCoord)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, tangent)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, bitangent)},
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================
BufferHandle CreateBuffer(BufferTarget target, BufferUsage usage, size_t size, const void* data)
{
	GLuint currentBuffer = GetCurrentBuffer(GetGLEnum(target));

	BufferHandle buffer{};
	glGenBuffers(1, &buffer.handle);
	glBindBuffer(GetGLEnum(target), buffer.handle);
	glBufferData(GetGLEnum(target), static_cast<GLsizeiptr>(size), data, GetGLEnum(usage));
	glBindBuffer(GetGLEnum(target), currentBuffer);

	return buffer;
}
//=============================================================================
void BufferSubData(BufferHandle bufferId, BufferTarget target, GLintptr offset, GLsizeiptr size, const void* data)
{
	GLuint currentBuffer = GetCurrentBuffer(GetGLEnum(target));

	glBindBuffer(GetGLEnum(target), bufferId.handle);
	glBufferSubData(GetGLEnum(target), offset, size, data);
	glBindBuffer(GetGLEnum(target), currentBuffer);
}
//=============================================================================
Texture1DHandle CreateTexture1D(unsigned width, InternalFormat internalformat, PixelFormat format, PixelType type, const void* pixels)
{
	if (width == 0 || format == PixelFormat::None)
	{
		Error("Invalid texture parameters");
		return {};
	}
	const GLuint currentTexture = GetCurrentTexture(GL_TEXTURE_1D);
	
	Texture1DHandle texture;
	glGenTextures(1, &texture.handle);
	glBindTexture(GL_TEXTURE_1D, texture.handle);
	glTexImage1D(GL_TEXTURE_1D, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), 0, EnumToValue(format), EnumToValue(type), pixels);
	
	glBindTexture(GL_TEXTURE_1D, currentTexture);
	return texture;
}
//=============================================================================
Texture2DHandle CreateTexture2D(unsigned width, unsigned height, InternalFormat internalformat, PixelFormat format, PixelType type, const void* pixels)
{
	if (width == 0 || height == 0 || format == PixelFormat::None)
	{
		Error("Invalid texture parameters");
		return {};
	}

	const GLuint currentTexture = GetCurrentTexture(GL_TEXTURE_2D);

	Texture2DHandle texture;
	glGenTextures(1, &texture.handle);
	glBindTexture(GL_TEXTURE_2D, texture.handle);
	glTexImage2D(GL_TEXTURE_2D, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), pixels);

	glBindTexture(GL_TEXTURE_2D, currentTexture);
	return texture;
}
//=============================================================================
Texture3DHandle CreateTexture3D(unsigned width, unsigned height, unsigned depth, InternalFormat internalformat, PixelFormat format, PixelType type, const void* pixels)
{
	if (width == 0 || height == 0 || depth == 0 || format == PixelFormat::None)
	{
		Error("Invalid texture parameters");
		return {};
	}
	const GLuint currentTexture = GetCurrentTexture(GL_TEXTURE_3D);
	
	Texture3DHandle texture;
	glGenTextures(1, &texture.handle);
	glBindTexture(GL_TEXTURE_3D, texture.handle);
	glTexImage3D(GL_TEXTURE_3D, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(depth), 0, EnumToValue(format), EnumToValue(type), pixels);

	glBindTexture(GL_TEXTURE_3D, currentTexture);
	return texture;
}
//=============================================================================
Texture1DArrayHandle CreateTexture1DArray(InternalFormat internalformat, unsigned width, unsigned arraySize, PixelFormat format, PixelType type, const void* pixels)
{
	if (width == 0 || arraySize == 0 || format == PixelFormat::None)
	{
		Error("Invalid texture parameters");
		return {};
	}
	const GLuint currentTexture = GetCurrentTexture(GL_TEXTURE_1D_ARRAY);
	Texture1DArrayHandle texture;
	glGenTextures(1, &texture.handle);
	glBindTexture(GL_TEXTURE_1D_ARRAY, texture.handle);
	glTexImage2D(GL_TEXTURE_1D_ARRAY, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(arraySize), 0, EnumToValue(format), EnumToValue(type), pixels);
	glBindTexture(GL_TEXTURE_1D_ARRAY, currentTexture);
	return texture;
}
//=============================================================================
Texture2DArrayHandle CreateTexture2DArray(InternalFormat internalformat, unsigned width, unsigned height, unsigned arraySize, PixelFormat format, PixelType type, const void* pixels)
{
	if (width == 0 || height == 0 || arraySize == 0 || format == PixelFormat::None)
	{
		Error("Invalid texture parameters");
		return {};
	}
	const GLuint currentTexture = GetCurrentTexture(GL_TEXTURE_2D_ARRAY);
	Texture2DArrayHandle texture;
	glGenTextures(1, &texture.handle);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture.handle);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(arraySize), 0, EnumToValue(format), EnumToValue(type), pixels);
	glBindTexture(GL_TEXTURE_2D_ARRAY, currentTexture);
	return texture;
}
//=============================================================================
TextureCubeHandle CreateCubeTexture(InternalFormat internalformat, unsigned width, unsigned height, PixelFormat format, PixelType type, const void* posX, const void* negX, const void* posY, const void* negY, const void* posZ, const void* negZ)
{
	if (width == 0 || height == 0 || format == PixelFormat::None)
	{
		Error("Invalid texture parameters");
		return {};
	}
	const GLuint currentTexture = GetCurrentTexture(GL_TEXTURE_CUBE_MAP);
	TextureCubeHandle texture;
	glGenTextures(1, &texture.handle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture.handle);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), posX);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), negX);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), posY);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), negY);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), posZ);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), negZ);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, currentTexture);
	return texture;
}
//=============================================================================
void SetTextureData(Texture1DHandle texture, InternalFormat internalformat, unsigned width, PixelFormat format, PixelType type, const void* pixels)
{
	if (width == 0 || format == PixelFormat::None || !pixels)
	{
		Error("Invalid texture parameters");
		return;
	}
	const GLuint currentTexture = GetCurrentTexture(GL_TEXTURE_1D);

	glBindTexture(GL_TEXTURE_1D, texture.handle);
	glTexImage1D(GL_TEXTURE_1D, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), 0, EnumToValue(format), EnumToValue(type), pixels);
	glBindTexture(GL_TEXTURE_1D, currentTexture);
}
//=============================================================================
void SetTextureData(Texture2DHandle texture, InternalFormat internalformat, unsigned width, unsigned height, PixelFormat format, PixelType type, const void* pixels)
{
	if (width == 0 || height == 0 || format == PixelFormat::None || !pixels)
	{
		Error("Invalid texture parameters");
		return;
	}

	const GLuint currentTexture = GetCurrentTexture(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture.handle);
	glTexImage2D(GL_TEXTURE_2D, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), pixels);
	glBindTexture(GL_TEXTURE_2D, currentTexture);
}
//=============================================================================
void SetTextureData(Texture3DHandle texture, InternalFormat internalformat, unsigned width, unsigned height, unsigned depth, PixelFormat format, PixelType type, const void* pixels)
{
	if (width == 0 || height == 0 || depth == 0 || format == PixelFormat::None || !pixels)
	{
		Error("Invalid texture parameters");
		return;
	}

	const GLuint currentTexture = GetCurrentTexture(GL_TEXTURE_3D);

	glBindTexture(GL_TEXTURE_3D, texture.handle);
	glTexImage3D(GL_TEXTURE_3D, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(depth), 0, EnumToValue(format), EnumToValue(type), pixels);
	glBindTexture(GL_TEXTURE_3D, currentTexture);
}
//=============================================================================
void SetTextureData(Texture1DArrayHandle texture, InternalFormat internalformat, unsigned width, unsigned arraySize, PixelFormat format, PixelType type, const void* pixels)
{
	if (width == 0 || arraySize == 0 || format == PixelFormat::None)
	{
		Error("Invalid texture parameters");
		return;
	}
	const GLuint currentTexture = GetCurrentTexture(GL_TEXTURE_1D_ARRAY);

	glBindTexture(GL_TEXTURE_1D_ARRAY, texture.handle);
	glTexImage2D(GL_TEXTURE_1D_ARRAY, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(arraySize), 0, EnumToValue(format), EnumToValue(type), pixels);
	glBindTexture(GL_TEXTURE_1D_ARRAY, currentTexture);
}
//=============================================================================
void SetTextureData(Texture2DArrayHandle texture, InternalFormat internalformat, unsigned width, unsigned height, unsigned arraySize, PixelFormat format, PixelType type, const void* pixels)
{
	if (width == 0 || height == 0 || arraySize == 0 || format == PixelFormat::None)
	{
		Error("Invalid texture parameters");
		return;
	}
	const GLuint currentTexture = GetCurrentTexture(GL_TEXTURE_2D_ARRAY);

	glBindTexture(GL_TEXTURE_2D_ARRAY, texture.handle);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(arraySize), 0, EnumToValue(format), EnumToValue(type), pixels);
	glBindTexture(GL_TEXTURE_2D_ARRAY, currentTexture);
}
//=============================================================================
void SetTextureData(TextureCubeHandle texture, InternalFormat internalformat, unsigned width, unsigned height, PixelFormat format, PixelType type, const void* posX, const void* negX, const void* posY, const void* negY, const void* posZ, const void* negZ)
{
	if (width == 0 || height == 0 || format == PixelFormat::None)
	{
		Error("Invalid texture parameters");
		return;
	}
	const GLuint currentTexture = GetCurrentTexture(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, texture.handle);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), posX);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), negX);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), posY);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), negY);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), posZ);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, EnumToValue(internalformat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, EnumToValue(format), EnumToValue(type), negZ);

	glBindTexture(GL_TEXTURE_CUBE_MAP, currentTexture);
}
//=============================================================================
void BindTexture2D(GLenum id, Texture2DHandle texture)
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, texture.handle);
}
//=============================================================================
bool IsValid(Texture1DHandle id)
{
	return id.handle > 0;
}
//=============================================================================
bool IsValid(Texture2DHandle id)
{
	return id.handle > 0;
}
//=============================================================================
bool IsValid(Texture3DHandle id)
{
	return id.handle > 0;
}
//=============================================================================
bool IsValid(Texture1DArrayHandle id)
{
	return id.handle > 0;
}
//=============================================================================
bool IsValid(Texture2DArrayHandle id)
{
	return id.handle > 0;
}
//=============================================================================
bool IsValid(TextureCubeHandle id)
{
	return id.handle > 0;
}
//=============================================================================
void Destroy(Texture1DHandle& id)
{
	glDeleteTextures(1, &id.handle);
	id.handle = 0;
}
//=============================================================================
void Destroy(Texture2DHandle& id)
{
	glDeleteTextures(1, &id.handle);
	id.handle = 0;
}
//=============================================================================
void Destroy(Texture3DHandle& id)
{
	glDeleteTextures(1, &id.handle);
	id.handle = 0;
}
//=============================================================================
void Destroy(Texture1DArrayHandle& id)
{
	glDeleteTextures(1, &id.handle);
	id.handle = 0;
}
//=============================================================================
void Destroy(Texture2DArrayHandle& id)
{
	glDeleteTextures(1, &id.handle);
	id.handle = 0;
}
//=============================================================================
void Destroy(TextureCubeHandle& id)
{
	glDeleteTextures(1, &id.handle);
	id.handle = 0;
}
//=============================================================================
inline void setTextureParameters(GLenum target, GLuint texture, const TextureConfig& config)
{
	if (texture == 0)
	{
		Error("Invalid texture handle");
		return;
	}

	const GLuint currentTexture = GetCurrentTexture(target);
	glBindTexture(target, texture);

	if (config.generateMipmaps)
		glGenerateMipmap(target);

	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GetGLEnum(config.minFilter));
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GetGLEnum(config.magFilter));
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GetGLEnum(config.wrapT));
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GetGLEnum(config.wrapS));
	glTexParameteri(target, GL_TEXTURE_WRAP_R, GetGLEnum(config.wrapR));

	glBindTexture(target, currentTexture);
}
//=============================================================================
void SetTextureParameters(Texture1DHandle texture, const TextureConfig& config)
{
	setTextureParameters(GL_TEXTURE_1D, texture.handle, config);
}
//=============================================================================
void SetTextureParameters(Texture2DHandle texture, const TextureConfig& config)
{
	setTextureParameters(GL_TEXTURE_2D, texture.handle, config);
}
//=============================================================================
void SetTextureParameters(Texture3DHandle texture, const TextureConfig& config)
{
	setTextureParameters(GL_TEXTURE_3D, texture.handle, config);
}
//=============================================================================
void SetTextureParameters(TextureCubeHandle texture, const TextureConfig& config)
{
	setTextureParameters(GL_TEXTURE_CUBE_MAP, texture.handle, config);
}
//=============================================================================
std::size_t std::hash<SamplerStateInfo>::operator()(const SamplerStateInfo& k) const noexcept
{
	auto rtup = std::make_tuple(
		k.maxAnisotropy,
		k.minLod,
		k.maxLod,
		k.mipLodBias,
		k.compareEnabled,
		k.comparisonFunc,
		k.borderColor[0],
		k.borderColor[1],
		k.borderColor[2],
		k.borderColor[3],
		k.minFilter,
		k.magFilter,
		k.wrapS,
		k.wrapT,
		k.wrapR);
	return Hash<decltype(rtup)>{}(rtup);
}
//=============================================================================
SamplerHandle CreateSamplerState(const SamplerStateInfo& info)
{
	if (auto it = SamplerCache.find(info); it != SamplerCache.end())
	{
		return it->second;
	}

	SamplerHandle sampler{ 0 };
	glGenSamplers(1, &sampler.handle);
	assert(sampler.handle);

	glSamplerParameteri(sampler.handle, GL_TEXTURE_MIN_FILTER, GetGLEnum(info.minFilter));
	glSamplerParameteri(sampler.handle, GL_TEXTURE_MAG_FILTER, GetGLEnum(info.magFilter));

	glSamplerParameteri(sampler.handle, GL_TEXTURE_WRAP_S, GetGLEnum(info.wrapS));
	glSamplerParameteri(sampler.handle, GL_TEXTURE_WRAP_T, GetGLEnum(info.wrapT));
	glSamplerParameteri(sampler.handle, GL_TEXTURE_WRAP_R, GetGLEnum(info.wrapR));

	if (info.maxAnisotropy > 1.0f && (GLAD_GL_ARB_texture_filter_anisotropic == 1))
	{
		// TODO: clamp maxAnisotropy(1, getCapabilities().maximumAnisotropy)
		glSamplerParameterf(sampler.handle, GL_TEXTURE_MAX_ANISOTROPY, info.maxAnisotropy);
	}

	glSamplerParameterf(sampler.handle, GL_TEXTURE_MIN_LOD, info.minLod);
	glSamplerParameterf(sampler.handle, GL_TEXTURE_MAX_LOD, info.maxLod);
	glSamplerParameterf(sampler.handle, GL_TEXTURE_LOD_BIAS, info.mipLodBias);

	if (info.compareEnabled)
	{
		glSamplerParameteri(sampler.handle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glSamplerParameteri(sampler.handle, GL_TEXTURE_COMPARE_FUNC, static_cast<GLint>(GetGLEnum(info.comparisonFunc)));
	}
	else
	{
		glSamplerParameteri(sampler.handle, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	}

	glSamplerParameterfv(sampler.handle, GL_TEXTURE_BORDER_COLOR, info.borderColor);

	return SamplerCache.insert({ info, sampler }).first->second;
}
//=============================================================================
GLuint CreateFramebuffer(GLuint colorTex, GLuint depthTex)
{
	GLuint fbo{ 0 };
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if (depthTex > 0)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
	if (colorTex > 0)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (GL_FRAMEBUFFER_COMPLETE != result)
	{
		Error("Framebuffer is not complete.");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fbo;
}
//=============================================================================
struct CachedState final
{
	bool restoreDepth{ true };
	bool restoreStencil{ true };
	bool restoreBlend{ true };
	bool restoreMultisample{ true };
	bool restoreColorMaskState{ true };
	bool restoreCullState{ true };
	bool restorePolygonState{ true };

	bool depthTestEnabled{ false };
	ComparisonFunc depthFunc{ ComparisonFunc::Less };
	bool depthMask{ true };

	bool stencilEnabled{ false };
	ComparisonFunc frontFunc{ ComparisonFunc::Always };
	ComparisonFunc backFunc{ ComparisonFunc::Always };
	int frontRef{ 0 };
	int backRef{ 0 };
	unsigned int frontMask{ 0xFF };
	unsigned int backMask{ 0xFF };

	bool blendEnabled{ false };
	BlendFactor srcRGB{ BlendFactor::SrcAlpha };
	BlendFactor dstRGB{ BlendFactor::OneMinusSrcAlpha };
	BlendFactor srcAlpha{ BlendFactor::One };
	BlendFactor dstAlpha{ BlendFactor::OneMinusSrcAlpha };

	bool multisampleEnabled{ true };

	bool colorMaskR{ true };
	bool colorMaskG{ true };
	bool colorMaskB{ true };
	bool colorMaskA{ true };

	bool cullEnabled{ false };
	CullFace cullFace{ CullFace::Back };

	PolygonMode polygonMode{ PolygonMode::Fill };
} cachedState;
//=============================================================================
void ResetStateDepth()
{
	cachedState.restoreDepth = true;
}
//=============================================================================
void ResetStateStencil()
{
	cachedState.restoreStencil = true;
}
//=============================================================================
void ResetStateBlend()
{
	cachedState.restoreBlend = true;
}
//=============================================================================
void ResetStateMultisample()
{
	cachedState.restoreMultisample = true;
}
//=============================================================================
void ResetStateColorMaskState()
{
	cachedState.restoreColorMaskState = true;
}
//=============================================================================
void ResetStateCullState()
{
	cachedState.restoreCullState = true;
}
//=============================================================================
void ResetStatePolygonState()
{
	cachedState.restorePolygonState = true;
}
//=============================================================================
void ResetStateAll()
{
	ResetStateDepth();
	ResetStateStencil();
	ResetStateBlend();
	ResetStateMultisample();
	ResetStateColorMaskState();
	ResetStateCullState();
	ResetStatePolygonState();
}
//=============================================================================
void BindState(const GLState& state)
{
	// Depth State
	if (cachedState.restoreDepth || cachedState.depthTestEnabled != state.depthState.enable)
	{
		if (state.depthState.enable) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
		cachedState.depthTestEnabled = state.depthState.enable;
	}
	if (cachedState.restoreDepth || cachedState.depthFunc != state.depthState.depthFunc)
	{
		glDepthFunc(GetGLEnum(state.depthState.depthFunc));
		cachedState.depthFunc = state.depthState.depthFunc;
	}
	if (cachedState.restoreDepth || cachedState.depthMask != state.depthState.depthMask)
	{
		glDepthMask(state.depthState.depthMask ? GL_TRUE : GL_FALSE);
		cachedState.depthMask = state.depthState.depthMask;
	}
	cachedState.restoreDepth = false;

	// Stencil State
	if (cachedState.restoreStencil || cachedState.stencilEnabled != state.stencilState.enable)
	{
		if (state.stencilState.enable) glEnable(GL_STENCIL_TEST);
		else glDisable(GL_STENCIL_TEST);
		cachedState.stencilEnabled = state.stencilState.enable;
	}
	if (cachedState.restoreStencil || 
		cachedState.frontFunc != state.stencilState.frontFunc || cachedState.backFunc != state.stencilState.backFunc ||
		cachedState.frontRef != state.stencilState.frontRef || cachedState.backRef != state.stencilState.backRef ||
		cachedState.frontMask != state.stencilState.frontMask || cachedState.backMask != state.stencilState.backMask)
	{
		glStencilFuncSeparate(GL_FRONT, GetGLEnum(state.stencilState.frontFunc), state.stencilState.frontRef, state.stencilState.frontMask);
		glStencilFuncSeparate(GL_BACK, GetGLEnum(state.stencilState.backFunc), state.stencilState.backRef, state.stencilState.backMask);
		cachedState.frontFunc = state.stencilState.frontFunc;
		cachedState.backFunc = state.stencilState.backFunc;
		cachedState.frontRef = state.stencilState.frontRef;
		cachedState.backRef = state.stencilState.backRef;
		cachedState.frontMask = state.stencilState.frontMask;
		cachedState.backMask = state.stencilState.backMask;
	}
	cachedState.restoreStencil = false;

	// Blend State
	if (cachedState.restoreBlend || cachedState.blendEnabled != state.blendState.enable)
	{
		if (state.blendState.enable) glEnable(GL_BLEND);
		else glDisable(GL_BLEND);
		cachedState.blendEnabled = state.blendState.enable;
	}
	if (cachedState.restoreBlend || 
		cachedState.srcRGB != state.blendState.srcRGB || cachedState.dstRGB != state.blendState.dstRGB ||
		cachedState.srcAlpha != state.blendState.srcAlpha || cachedState.dstAlpha != state.blendState.dstAlpha)
	{
		glBlendFuncSeparate(
			GetGLEnum(state.blendState.srcRGB),
			GetGLEnum(state.blendState.dstRGB),
			GetGLEnum(state.blendState.srcAlpha),
			GetGLEnum(state.blendState.dstAlpha)
		);
		cachedState.srcRGB = state.blendState.srcRGB;
		cachedState.dstRGB = state.blendState.dstRGB;
		cachedState.srcAlpha = state.blendState.srcAlpha;
		cachedState.dstAlpha = state.blendState.dstAlpha;
	}
	cachedState.restoreBlend = false;

	// Multisample State
	if (cachedState.restoreMultisample || cachedState.multisampleEnabled != state.multisampleState.enable)
	{
		if (state.multisampleState.enable) glEnable(GL_MULTISAMPLE);
		else glDisable(GL_MULTISAMPLE);
		cachedState.multisampleEnabled = state.multisampleState.enable;
	}
	cachedState.restoreMultisample = false;

	// Color Mask
	if (cachedState.restoreColorMaskState || 
		cachedState.colorMaskR != state.colorMaskState.r || cachedState.colorMaskG != state.colorMaskState.g ||
		cachedState.colorMaskB != state.colorMaskState.b || cachedState.colorMaskA != state.colorMaskState.a)
	{
		glColorMask(state.colorMaskState.r ? GL_TRUE : GL_FALSE,
			state.colorMaskState.g ? GL_TRUE : GL_FALSE,
			state.colorMaskState.b ? GL_TRUE : GL_FALSE,
			state.colorMaskState.a ? GL_TRUE : GL_FALSE);
		cachedState.colorMaskR = state.colorMaskState.r;
		cachedState.colorMaskG = state.colorMaskState.g;
		cachedState.colorMaskB = state.colorMaskState.b;
		cachedState.colorMaskA = state.colorMaskState.a;
	}
	cachedState.restoreColorMaskState = false;
		
	// Cull State
	if (cachedState.restoreCullState || cachedState.cullEnabled != state.cullState.enable)
	{
		if (state.cullState.enable) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		cachedState.cullEnabled = state.cullState.enable;
	}
	if (cachedState.restoreCullState || cachedState.cullFace != state.cullState.cullFace)
	{
		glCullFace(GetGLEnum(state.cullState.cullFace));
		cachedState.cullFace = state.cullState.cullFace;
	}
	cachedState.restoreCullState = false;

	// Polygon Mode
	if (cachedState.restorePolygonState || cachedState.polygonMode != state.polygonState.mode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GetGLEnum(state.polygonState.mode));
		cachedState.polygonMode = state.polygonState.mode;
	}
	cachedState.restorePolygonState = false;
}
//=============================================================================
GLuint GetCurrentBuffer(GLenum target)
{
	GLenum targetBinding{ 0 };
	switch (target)
	{
	case GL_ARRAY_BUFFER:         targetBinding = GL_ARRAY_BUFFER_BINDING; break;
	case GL_ELEMENT_ARRAY_BUFFER: targetBinding = GL_ELEMENT_ARRAY_BUFFER_BINDING; break;
	case GL_UNIFORM_BUFFER:       targetBinding = GL_UNIFORM_BUFFER_BINDING; break;
	default: std::unreachable(); break;
	}
	GLuint currentBuffer{ 0 };
	glGetIntegerv(targetBinding, (GLint*)&currentBuffer);

	return currentBuffer;
}
//=============================================================================
GLuint GetCurrentTexture(GLenum target)
{
	GLenum targetBinding{ 0 };
	switch (target)
	{
	case GL_TEXTURE_1D:                   targetBinding = GL_TEXTURE_BINDING_1D; break;
	case GL_TEXTURE_2D:                   targetBinding = GL_TEXTURE_BINDING_2D; break;
	case GL_TEXTURE_3D:                   targetBinding = GL_TEXTURE_BINDING_3D; break;
	case GL_TEXTURE_1D_ARRAY:             targetBinding = GL_TEXTURE_BINDING_1D_ARRAY; break;
	case GL_TEXTURE_2D_ARRAY:             targetBinding = GL_TEXTURE_BINDING_2D_ARRAY; break;
	case GL_TEXTURE_2D_MULTISAMPLE:       targetBinding = GL_TEXTURE_BINDING_2D_MULTISAMPLE; break;
	case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: targetBinding = GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY; break;
	case GL_TEXTURE_BUFFER:               targetBinding = GL_TEXTURE_BINDING_BUFFER; break;
	case GL_TEXTURE_CUBE_MAP:             targetBinding = GL_TEXTURE_BINDING_CUBE_MAP; break;
	case GL_TEXTURE_RECTANGLE:            targetBinding = GL_TEXTURE_BINDING_RECTANGLE; break;

	default: std::unreachable(); break;
	}
	GLuint currentState{ 0 };
	glGetIntegerv(targetBinding, (GLint*)&currentState);
	return currentState;
}
//=============================================================================
void EnableSRGB(bool enable)
{
#if ENABLE_SRGB
	if (enable) glEnable(GL_FRAMEBUFFER_SRGB);
	else        glDisable(GL_FRAMEBUFFER_SRGB);
#else
	(void)enable;
#endif
}
//=============================================================================
void DrawArrays(GLuint vao, GLenum mode, GLint first, GLsizei count)
{
	glBindVertexArray(vao);
	glDrawArrays(mode, first, count);
}
//=============================================================================
void DrawElements(GLuint vao, GLenum mode, GLsizei count, GLenum type, const void* indices)
{
	glBindVertexArray(vao);
	glDrawElements(mode, count, type, indices);
}
//=============================================================================