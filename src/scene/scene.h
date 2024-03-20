#pragma once

#include <memory>

#include "terrain.h"

class Scene
{
public:
	Scene() = default;

public:
	std::unique_ptr<Terrain> m_Terrain = nullptr;
	std::shared_ptr<std::vector<std::shared_ptr<VAO>>> m_VAOs = std::make_shared<std::vector<std::shared_ptr<VAO>>>();
};