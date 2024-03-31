#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "primitives/sphere.h"
#include "primitives/heightfield.h"

inline static const glm::vec3 GRAVITY = glm::vec3(0.0f, -9.81f, 0.0f);

class Simulator
{
public:
	Simulator() { Init(); }
	Simulator(uint32_t width, uint32_t length, uint32_t depth) : m_WorldWidth(width), m_WorldLength(length), m_WorldDepth(depth) { Init(); }
	~Simulator() = default;

	void SetDeltaTime(float deltaTime) { m_DeltaTime = deltaTime; }
	float GetDeltaTime() const { return m_DeltaTime; }

	void SetWorldBounds(uint32_t width, uint32_t length, uint32_t depth)
	{
		m_WorldWidth = width;
		m_WorldLength = length;
		m_WorldDepth = depth;
	}

	void SetWorldWidth(uint32_t width) { m_WorldWidth = width; }
	uint32_t GetWorldWidth() const { return m_WorldWidth; }

	void SetWorldLength(uint32_t length) { m_WorldLength = length; }
	uint32_t SetWorldLength() const { return m_WorldLength; }

	void SetWorldDepth(uint32_t depth) { m_WorldDepth = depth; }
	uint32_t SetWorldDepth() const { return m_WorldDepth; }

	void SetTerrain(std::vector<glm::vec3> vertices)
	{
		if (!m_TerrainGeometry)
			m_TerrainGeometry = std::make_unique<Heightfield>(m_WorldLength, m_WorldWidth);

		m_TerrainGeometry->SetHeightsFromTraingleMesh(vertices);
	}

	void SetIDs(const std::shared_ptr<std::vector<uint64_t>>& IDs) { m_IDs = IDs; }
	std::shared_ptr<std::vector<uint64_t>> GetIDs() const { return m_IDs; }

	void SetIDsToCenters(const std::shared_ptr<std::unordered_map<uint64_t, glm::vec3>>& IDsToCenters) { m_IDsToCenters = IDsToCenters; }
	std::shared_ptr<std::unordered_map<uint64_t, glm::vec3>> GetIDsToCenters() const { return m_IDsToCenters; }

	DynamicSphere& GetParticle(const uint64_t id) { return m_IDsToParticles[id]; }
	glm::vec3& GetCenter(const uint64_t id) { return m_IDsToCenters->at(id); }

	void RegisterParticles(const std::vector<std::pair<uint64_t, DynamicSphere>>& IDsToSpheres, float radius)
	{
		for (const auto& [id, sphere] : IDsToSpheres)
		{
			RegisterParticle(id, sphere);
		}
	}

	void RegisterParticle(const uint64_t ID, const DynamicSphere& particle)
	{
		m_IDsToParticles[ID] = particle;
		GetParticleBlock(particle).push_back(particle);
	}

	void ClearParticles() { m_IDsToParticles.clear(); }
	void ClearParticleGrid()
	{
		for (auto& row : m_ParticleGrid)
		{
			for (auto& col : row)
			{
				for (auto& aisle : col)
				{
					aisle.clear();
				}
			}
		}
	}

	void Step()
	{
		HandleCollisions();
		ApplySPH();
	}

private:
	void Init()
	{
		m_ParticleGrid = std::vector<std::vector<std::vector<std::vector<DynamicSphere>>>>(m_WorldDepth, std::vector<std::vector<std::vector<DynamicSphere>>>(m_WorldLength, std::vector<std::vector<DynamicSphere>>(m_WorldWidth)));
	}

	uint32_t GetParticleLengthIndex(const DynamicSphere& particle) const { return static_cast<uint32_t>(particle.GetPosition().z) + m_WorldLength / 2U; }
	uint32_t GetParticleWidthIndex(const DynamicSphere& particle) const { return static_cast<uint32_t>(particle.GetPosition().x) + m_WorldWidth / 2U; }
	uint32_t GetParticleDepthIndex(const DynamicSphere& particle) const { return static_cast<uint32_t>(particle.GetPosition().y) + m_WorldDepth / 2U; }

	std::vector<DynamicSphere>& GetParticleBlock(const DynamicSphere& particle) { return m_ParticleGrid[GetParticleDepthIndex(particle)][GetParticleWidthIndex(particle)][GetParticleLengthIndex(particle)]; }

	std::vector<DynamicSphere> GetParticleNeighbors(const DynamicSphere& particle);

	void HandleCollisions();
	void ApplySPH();

	void ApplyForces(DynamicSphere& particle, const std::vector<DynamicSphere>& neighbors);

	float GetDensity(const DynamicSphere& particle, const std::vector<DynamicSphere>& neighbors);

	void ApplyPreassureForce(DynamicSphere& particle, const std::vector<DynamicSphere>& neighbors);
	void ApplyExternalForce(DynamicSphere& particle, const std::vector<DynamicSphere>& neighbors);
	// TODO
	void ApplySurfaceForce(DynamicSphere& particle, const std::vector<DynamicSphere>& neighbors);
	void ApplyViscosityForce(DynamicSphere& particle, const std::vector<DynamicSphere>& neighbors);

	float Wpoly(float distance)
	{
		if (0.0f > distance || distance > 1.0f)
			return 0.0f;

		return static_cast<float>(1.566681471 * pow(1.0 - distance * distance, 3));
	}

	// TODO
	float Wspiky1stOrder(float distance) { return 0.0f; }

	float Wviscosity2ndOrder(float distance)
	{
		if (0.0f > distance || distance > 1.0f)
			return 0.0f;

		return static_cast<float>(14.32394488 * (1.0 - distance));
	}

private:
	std::unique_ptr<Heightfield> m_TerrainGeometry = nullptr;
	std::shared_ptr<std::vector<uint64_t>> m_IDs = nullptr;
	std::shared_ptr<std::unordered_map<uint64_t, glm::vec3>> m_IDsToCenters = nullptr;
	std::unordered_map<uint64_t, DynamicSphere> m_IDsToParticles = std::unordered_map<uint64_t, DynamicSphere>();
	std::vector<std::vector<std::vector<std::vector<DynamicSphere>>>> m_ParticleGrid = std::vector<std::vector<std::vector<std::vector<DynamicSphere>>>>();
	uint32_t m_WorldLength = 0U;
	uint32_t m_WorldWidth  = 0U;
	uint32_t m_WorldDepth  = 0U;
	float m_DeltaTime      = 0.0f;
};
