#pragma once

#include <memory>
#include <unordered_map>

#include "..\glObjects.h"
#include "shaderProgram.h"

class Renderer
{
public:
	Renderer() = default;

	void SetVAOs(std::shared_ptr<std::vector<std::shared_ptr<VAO>>> VAOs) { m_VAOs = VAOs; }

	void Render();

	template<typename... Args>
	std::shared_ptr<ShaderProgram> AddShaderProgram(Args... args)
	{
		std::shared_ptr shaderProgram = std::make_shared<ShaderProgram>(args...);
		m_ShaderPrograms.push_back(shaderProgram);
		return shaderProgram;
	}

	void RegisterVAOShaderMatch(GLuint id, GLuint sp) { m_VAOtoShaderProgram.insert({ id, sp }); }

private:
	std::shared_ptr<std::vector<std::shared_ptr<VAO>>> m_VAOs = nullptr;
	std::vector<std::shared_ptr<ShaderProgram>> m_ShaderPrograms = std::vector<std::shared_ptr<ShaderProgram>>();
	std::unordered_map<GLuint, GLuint> m_VAOtoShaderProgram = {};
};

