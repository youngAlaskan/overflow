#pragma once

#include <memory>

#include "terrain.h"

class Scene
{
public:
	Scene() = default;

public:
	Terrain m_Terrain = Terrain();
	std::vector<VAO> m_VAOs = std::vector<VAO>();
};