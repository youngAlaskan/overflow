#pragma once

#include <unordered_map>

#include "..\glObjects.h"
#include "shaderProgram.h"

class Renderer
{
public:
	Renderer() = default;
	void Render();

public:
	std::vector<VAO> m_VAOs = {};
	std::unordered_map<VAO, ShaderProgram> m_VAOtoShaderProgram = {};
};

