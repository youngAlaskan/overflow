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

	void RegisterVAOShaderMatch(GLuint id, const ShaderProgram& sp) { m_VAOtoShaderProgram.insert({ id, sp }); }

private:
	std::shared_ptr<std::vector<std::shared_ptr<VAO>>> m_VAOs = nullptr;
	std::unordered_map<GLuint, ShaderProgram> m_VAOtoShaderProgram = {};
};

