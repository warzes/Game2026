#pragma once

//=============================================================================
// Object Handles
//=============================================================================
struct ProgramHandle final { GLuint handle{ 0u }; };

//=============================================================================
// Shader Program
//=============================================================================
std::string LoadShaderCode(const std::string& filename, const std::vector<std::string>& defines = {});

ProgramHandle CreateShaderProgram(std::string_view vertexShader);
ProgramHandle CreateShaderProgram(std::string_view vertexShader, std::string_view fragmentShader);
ProgramHandle CreateShaderProgram(std::string_view vertexShader, std::string_view geometryShader, std::string_view fragmentShader);

ProgramHandle LoadShaderProgram(const std::string& vsFile, const std::vector<std::string>& defines = {});
ProgramHandle LoadShaderProgram(const std::string& vsFile, const std::string& fsFile, const std::vector<std::string>& defines = {});
ProgramHandle LoadShaderProgram(const std::string& vsFile, const std::string& gsFile, const std::string& fsFile, const std::vector<std::string>& defines = {});

//=============================================================================
// Shader Uniforms
//=============================================================================
int GetUniformLocation(ProgramHandle program, std::string_view name);

void SetUniform(int id, bool b);
void SetUniform(int id, float s);
void SetUniform(int id, int s);
void SetUniform(int id, unsigned s);
void SetUniform(int id, const glm::vec2& v);
void SetUniform(int id, std::span<const glm::vec2> v);
void SetUniform(int id, const glm::vec3& v);
void SetUniform(int id, std::span<const glm::vec3> v);
void SetUniform(int id, const glm::vec4& v);
void SetUniform(int id, std::span<const glm::vec4> v);
void SetUniform(int id, const glm::quat& v);
void SetUniform(int id, const glm::mat3& m);
void SetUniform(int id, const glm::mat4& m);