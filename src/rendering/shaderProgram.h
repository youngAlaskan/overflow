#pragma once

#include <glad\glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <utility>
#include <string>

class ShaderProgram
{
public:
	ShaderProgram() { m_ID = glCreateProgram(); }
	~ShaderProgram() { glDeleteProgram(m_ID); }

	void AttachShaders(std::vector<std::pair<GLenum, const char *>> shaderTypesAndFilepaths) const;

	void Use() const { glUseProgram(m_ID); }

	void SetBool(const char* name, const bool value) const;
    void SetInt(const char* name, const int value) const;
    void SetFloat(const char* name, const float value) const;
    void SetVec2(const char* name, const glm::vec2& v) const;
    void SetVec3(const char* name, const glm::vec3& v) const;
    void SetVec4(const char* name, const glm::vec4& v) const;
	void SetMat2(const char* name, const glm::mat2& m) const;
    void SetMat3(const char* name, const glm::mat3& m) const;
    void SetMat4(const char* name, const glm::mat4& m) const;

private:
	std::string ReadFile(const char* filepath) const;

	void CheckShaderCompilation(const GLuint shader) const;
	void CheckProgramCompilation() const;

private:
	GLuint m_ID;
};