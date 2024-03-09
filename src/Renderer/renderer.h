#pragma once

#include "..\glObjects.h"

class Renderer
{
public:
	Renderer() = default;
	void Render();

public:
	std::vector<VAO> m_VAOs = {};
};

