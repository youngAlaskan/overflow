#pragma once

#include <memory>

#include "camera.h"
#include "terrain.h"

class Scene
{
public:
	Scene() = default;

	// Creates a terrain with the given arguments, and pushes the terrain's VAO
	// on to m_VAOs.
	template<typename... Args>
	void SetTerrain(Args... args)
	{
		m_Terrain = std::make_unique<Terrain>(args...);
		m_VAOs->push_back(m_Terrain->m_VAO);
	}

	void OnUpdate() const
	{
		m_Camera->OnUpdate();
	}

public:
	std::shared_ptr<Camera> m_Camera = std::make_shared<Camera>();
	std::unique_ptr<Terrain> m_Terrain = nullptr;
	std::shared_ptr<std::vector<std::shared_ptr<VAO>>> m_VAOs = std::make_shared<std::vector<std::shared_ptr<VAO>>>();
};