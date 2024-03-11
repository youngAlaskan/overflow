#pragma once

#include <memory>

#include "terrain.h"

class Scene
{
public:
	Scene() = default;

	std::shared_ptr<std::vector<VAO>> GetVAOs() const { return m_VAOs; }

	void SetTerrain(const Terrain& terrain)
	{
		m_Terrain = terrain;
		m_VAOs->push_back(m_Terrain.m_VAO);
	}

public:
	Terrain m_Terrain;
	
private:
	std::shared_ptr<std::vector<VAO>> m_VAOs = {};
};