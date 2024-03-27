#pragma once

#include <memory>

#include "camera.h"
#include "terrain.h"
#include "droplets.h"

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

	void UpdateTerrain(std::vector<Vertex> vertices) const
	{
		m_Terrain->SetVertices(vertices);
	}

	template<typename... Args>
	void CreateDroplets(Args... args)
	{
		m_Droplets = std::make_unique<Droplets>(args...);
		m_VAOs->push_back(m_Droplets->m_VAO);
	}

	void UpdateDroplets(std::vector<Droplet> droplets) const
	{
		m_Droplets->SetInstanceVBO(droplets);
	}

	void OnUpdate() const
	{
		m_Camera->OnUpdate();
	}

public:
	std::shared_ptr<Camera> m_Camera = std::make_shared<Camera>();
	std::unique_ptr<Terrain> m_Terrain = nullptr;
	std::unique_ptr<Droplets> m_Droplets = nullptr;
	std::shared_ptr<std::vector<std::shared_ptr<VAO>>> m_VAOs = std::make_shared<std::vector<std::shared_ptr<VAO>>>();
};