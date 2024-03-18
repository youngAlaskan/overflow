#pragma once

#include <memory>

#include "terrain.h"

class Scene
{
public:
	Scene() = default;

public:
	Terrain m_Terrain = Terrain();
	std::shared_ptr<std::vector<VAO>> m_VAOs = std::make_shared<std::vector<VAO>>();
};