#include "shaderProgram.h"

#include <fstream>
#include <iostream>
#include <assert.h>

#include <glm/gtc/type_ptr.hpp>

void ShaderProgram::AttachShaders(std::vector<std::pair<GLenum, const char*>> shaderTypesAndFilepaths) const
{
    int limit = static_cast<int>(shaderTypesAndFilepaths.size());
    std::vector<GLuint> shaderIDs = std::vector<GLuint>(limit, 0);
    
    for (int i = 0; i < limit; i++)
    {
        const auto& [shader, filepath] = shaderTypesAndFilepaths[i];
        shaderIDs[i] = glCreateShader(shader);
        const char* code = ReadFile(filepath);
        
        glShaderSource(shaderIDs[i], 1, &code, nullptr);
        
        glCompileShader(shaderIDs[i]);
        CheckShaderCompilation(shaderIDs[i]);

        glAttachShader(m_ID, shaderIDs[i]);
    }

    glLinkProgram(m_ID);
    CheckProgramCompilation();

    for (GLuint shaderID : shaderIDs)
    {
        glDeleteShader(shaderID);
    }
}

void ShaderProgram::SetBool(const char* name, const bool value) const
{
    GLint location = glGetUniformLocation(m_ID, name);
    assert(location != -1);
    glUniform1i(location, static_cast<int>(value));
}

void ShaderProgram::SetInt(const char* name, const int value) const
{
    GLint location = glGetUniformLocation(m_ID, name);
    assert(location != -1);
    glUniform1i(location, value);
}

void ShaderProgram::SetFloat(const char* name, const float value) const
{
    GLint location = glGetUniformLocation(m_ID, name);
    assert(location != -1);
    glUniform1f(location, value);
}

void ShaderProgram::SetVec2(const char* name, const glm::vec2& v) const
{
    GLint location = glGetUniformLocation(m_ID, name);
    assert(location != -1);
    glUniform2fv(location, 1, glm::value_ptr(v));
}

void ShaderProgram::SetVec3(const char* name, const glm::vec3& v) const
{
    GLint location = glGetUniformLocation(m_ID, name);
    assert(location != -1);
    glUniform3fv(location, 1, glm::value_ptr(v));
}

void ShaderProgram::SetVec4(const char* name, const glm::vec4& v) const
{
    GLint location = glGetUniformLocation(m_ID, name);
    assert(location != -1);
    glUniform4fv(location, 1, glm::value_ptr(v));
}

void ShaderProgram::SetMat2(const char* name, const glm::mat2& m) const
{
    GLint location = glGetUniformLocation(m_ID, name);
    assert(location != -1);
    glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::SetMat3(const char* name, const glm::mat3& m) const
{
    GLint location = glGetUniformLocation(m_ID, name);
    assert(location != -1);
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::SetMat4(const char* name, const glm::mat4& m) const
{
    GLint location = glGetUniformLocation(m_ID, name);
    assert(location != -1);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
}

const char* ShaderProgram::ReadFile(const char* filepath) const
{
    try
    {
        std::ifstream fileStream(filepath);
        fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        return std::string(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>()).c_str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR: Could not read file: " << e.what() << std::endl;
    }

    return {};
}

void ShaderProgram::CheckShaderCompilation(const uint32_t shader) const
{
    int success = 0;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION:\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}

void ShaderProgram::CheckProgramCompilation() const
{
    int success = 0;
    char infoLog[1024];
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_ID, 1024, nullptr, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING:\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}