#pragma once

#include <memory>
#include <unordered_map>

#include "..\glObjects.h"
#include "shaderProgram.h"

class Renderer
{
public:
	Renderer() = default;

	void SetVAOs(std::shared_ptr<std::vector<VAO>> VAOs) { m_VAOs = VAOs; }

	void Render();

private:
	std::shared_ptr<std::vector<VAO>> m_VAOs = nullptr;
	std::unordered_map<GLuint, ShaderProgram> m_VAOtoShaderProgram = {};
};

