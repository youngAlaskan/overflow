#pragma once

#include <glad\glad.h>

#include <vector>
#include <utility>

class ShaderProgram
{
public:
	ShaderProgram() { m_ID = glCreateProgram(); }
	~ShaderProgram() { glDeleteProgram(m_ID); }

	void AttachShaders(std::vector<std::pair<GLenum, const char *>> shaderTypesAndFilepaths) const;

	void Use() const { glUseProgram(m_ID); }

private:
	const char* ReadFile(const char* filepath) const;

	void CheckShaderCompilation(const uint32_t shader) const;
	void CheckProgramCompilation() const;

private:
	GLuint m_ID;
};