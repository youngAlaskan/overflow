#pragma once

#include <memory>

#include "camera.h"
#include "terrain.h"
#include "droplets.h"
#include "../physics/primitives/heightfield.h"

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

	void UpdateTerrain(const std::vector<Vertex>& vertices) const
	{
		m_Terrain->SetVertices(vertices);
	}

	void UpdateTerrainWaterLevels() const
	{
		auto& vertices = m_Terrain->m_Vertices;
		for (auto& vertex : vertices)
		{
			auto position = vertex.Position;
			vertex.WaterLevel = m_WaterLevels->GetHeight(position.x, position.z);
		}
		UpdateTerrain(vertices);
	}

	template<typename... Args>
	void CreateDroplets(Args... args)
	{
		m_Droplets = std::make_unique<Droplets>(args...);
		m_VAOs->push_back(m_Droplets->GetVAO());
	}

	void UpdateDroplets() const
	{
		m_Droplets->UpdateInstanceVBO(*m_IDs);
	}

	void OnUpdate() const
	{
		m_Camera->OnUpdate();
		UpdateDroplets();
		UpdateTerrainWaterLevels();
	}

	void RegisterParticle(const std::pair<uint64_t, glm::vec3>& IDandCenter) const
	{
		m_Droplets->AddDroplet(IDandCenter);
		const auto& [id, center] = IDandCenter;
		m_IDs->push_back(id);
	}

	void RegisterParticles(const std::vector<std::pair<uint64_t, glm::vec3>>& IDandCenters) const
	{
		m_Droplets->AddDroplets(IDandCenters);
		for (const auto& [id, center] : IDandCenters)
		{
			m_IDs->push_back(id);
		}
	}

	static uint64_t GetFreshUUID()
	{
		static uint64_t uuid = 1ULL;
		return uuid++;
	}

public:
	std::shared_ptr<Camera> m_Camera = std::make_shared<Camera>();
	std::unique_ptr<Terrain> m_Terrain = nullptr;
	std::shared_ptr<Heightfield> m_WaterLevels = nullptr;
	std::unique_ptr<Droplets> m_Droplets = nullptr;
	std::shared_ptr<std::vector<uint64_t>> m_IDs = std::make_shared<std::vector<uint64_t>>();
	std::shared_ptr<std::vector<std::shared_ptr<VAO>>> m_VAOs = std::make_shared<std::vector<std::shared_ptr<VAO>>>();
};