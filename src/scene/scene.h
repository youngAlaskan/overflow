#pragma once

#include <memory>

#include "terrain.h"

class Scene
{
public:
	Scene() {}

	std::shared_ptr<std::vector<VAO>> GetVAOs() const { return m_VAOs; }

public:
	Terrain m_Terrain = Terrain();
	
private:
	std::shared_ptr<std::vector<VAO>> m_VAOs = {};
};