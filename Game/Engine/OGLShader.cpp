#include "stdafx.h"
#include "OGLShader.h"
#include "NanoLog.h"
//=============================================================================
std::string loadShaderCode(const std::string& path, unsigned int level);
//=============================================================================
inline std::string preprocessShaderCode(const std::string& line, const std::string& directory, unsigned int level)
{
	static const std::regex re("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
	std::smatch matches;

	if (regex_search(line, matches, re))
	{
		std::string path = matches[1].str();
		return loadShaderCode(directory + "/" + path, level);
	}
	else
	{
		return line;
	}
}
//=============================================================================
inline std::string loadShaderCode(const std::string& path, unsigned int level)
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

	GLint compileStatus{ 0 };
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
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