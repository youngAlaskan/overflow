#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <set>
#include <cstdint>

#include <algorithm>
#include <cmath>

#include "primitives/sphere.h"
#include "primitives/heightgrid.h"

inline static const glm::vec3 GRAVITY = glm::vec3(0.0f, -9.81f, 0.0f);

struct CellIndex { uint32_t row; uint32_t col; uint32_t aisle; };

class Simulator
{
public:
	Simulator() { Init(); }
	Simulator(uint32_t width, uint32_t length, uint32_t depth) : m_WorldWidth(width), m_WorldLength(length), m_WorldDepth(depth) { Init(); }

	void SetDeltaTime(float deltaTime) { m_DeltaTime = deltaTime; }
	float GetDeltaTime() const { return m_DeltaTime; }

	void SetWorldBounds(uint32_t width, uint32_t length, uint32_t depth)
	{
		m_WorldWidth = width;
		m_WorldLength = length;
		m_WorldDepth = depth;
	}

	void SetWorldWidth(uint32_t width) { m_WorldWidth = width; Init(); }
	uint32_t GetWorldWidth() const { return m_WorldWidth; }

	void SetWorldLength(uint32_t length) { m_WorldLength = length; Init(); }
	uint32_t GetWorldLength() const { return m_WorldLength; }

	void SetWorldDepth(uint32_t depth) { m_WorldDepth = depth; Init(); }
	uint32_t GetWorldDepth() const { return m_WorldDepth; }

	void SetTerrain(std::vector<glm::vec3> vertices)
	{
		m_TerrainGeometry->Build(vertices);
		Init();
	}

	float GetTerrainHeight(float x, float z) const { return m_TerrainGeometry->GetHeight(x, z); }

	void SetIDs(const std::shared_ptr<std::vector<uint64_t>>& IDs) { m_IDs = IDs; }
	std::shared_ptr<std::vector<uint64_t>> GetIDs() const { return m_IDs; }

	void SetIDsToCenters(const std::shared_ptr<std::unordered_map<uint64_t, glm::vec3>>& IDsToCenters) { m_IDsToCenters = IDsToCenters; }
	std::shared_ptr<std::unordered_map<uint64_t, glm::vec3>> GetIDsToCenters() const { return m_IDsToCenters; }

	DynamicSphere& GetParticle(const uint64_t id) { return m_IDsToParticles.at(id); }
	glm::vec3& GetCenter(const uint64_t id) { return m_IDsToCenters->at(id); }

	float ComputeParticleMass() const
	{
		const int n = static_cast<int>(std::ceil(H / m_ParticleSpacing));
		float sum = 0.0f;
		for (int i = -n; i <= n; i++)
			for (int j = -n; j <= n; j++)
				for (int k = -n; k <= n; k++)
					sum += Wpoly(m_ParticleSpacing * std::sqrt(float(i * i + j * j + k * k)));  // includes self at (0,0,0)
		return m_RestDensity / sum;
	}

	void RegisterParticles(const std::vector<std::pair<uint64_t, DynamicSphere>>& IDsToSpheres, float radius)
	{
		for (const auto& [id, sphere] : IDsToSpheres)
		{
			RegisterParticle(id, sphere);
		}
	}

	void RegisterParticle(const uint64_t ID, const DynamicSphere& particle)
	{
		DynamicSphere p = particle;
		p.SetMass(ComputeParticleMass());
		m_IDsToParticles[ID] = p;
	}

	void RemoveParticle(const uint64_t ID)
	{
		DynamicSphere& particle = m_IDsToParticles.at(ID);

		// Remove center mapping if present
		if (m_IDsToCenters)
			m_IDsToCenters->erase(ID);
		m_IDsToParticles.erase(ID);
	}

	void ClearParticles() { m_IDsToParticles.clear(); }

	void SetParticleGrid();
	void ClearParticleGrid()
	{
		for (const auto& idx : m_OccupiedCells)
		{
			m_ParticleGrid[idx.row][idx.col][idx.aisle].clear();
		}
		m_OccupiedCells.clear();
	}
	void UpdateParticleGrid()
	{
		ClearParticleGrid();
		SetParticleGrid();
	}

	void SetNeighbors();
	void ClearNeighbors()
	{
		m_Neighbors.clear();
	}
	void UpdateNeighbors()
	{
		ClearNeighbors();
		SetNeighbors();
	}

	void Step()
	{
		if (!m_IDs || m_IDs->empty() || m_DeltaTime == 0.0f)
			return;
		UpdateParticleGrid();
		UpdateNeighbors();
		ApplySPH();				// density -> forces -> integrate -> HandleCollisions -> sync centers
		ClearNeighbors();
	}

	~Simulator() {
		ClearParticles();
		ClearParticleGrid();
		ClearNeighbors();

		if (m_TerrainGeometry)
			m_TerrainGeometry.reset();
	}
public:
	inline static constexpr float H = 1.0f;   // smoothing radius (matches grid cell size)
	inline static constexpr float PI = 3.14159265f;

private:
	void Init()
	{
		m_OccupiedCells.clear();
		m_ParticleGrid.assign(m_WorldDepth + 1U,
			std::vector<std::vector<std::vector<uint64_t>>>(m_WorldWidth + 1U,
				std::vector<std::vector<uint64_t>>(m_WorldLength + 1U)));
	}

	uint32_t GetParticleLengthIndex(const DynamicSphere& particle) const { return static_cast<uint32_t>(particle.GetPosition().z + m_WorldLength / (H * 2.0f)); }
	uint32_t GetParticleWidthIndex(const DynamicSphere& particle) const { return static_cast<uint32_t>(particle.GetPosition().x + m_WorldWidth / (H * 2.0f)); }
	uint32_t GetParticleDepthIndex(const DynamicSphere& particle) const { return static_cast<uint32_t>(particle.GetPosition().y + m_WorldDepth / (H * 2.0f)); }

	std::vector<uint64_t>& GetParticleBlock(const DynamicSphere& particle){ return m_ParticleGrid[GetParticleDepthIndex(particle)][GetParticleWidthIndex(particle)][GetParticleLengthIndex(particle)]; }
	std::vector<uint64_t>& GetParticleBlock(const uint64_t id)
	{
		DynamicSphere& particle = GetParticle(id);
		return m_ParticleGrid[GetParticleDepthIndex(particle)][GetParticleWidthIndex(particle)][GetParticleLengthIndex(particle)];
	}

	std::vector<uint64_t> GetParticleNeighbors(const uint64_t id);

	bool IsParticleInWorldBounds(const DynamicSphere& particle) const;

	void CullOutOfBounds()
	{
		std::vector<uint64_t> alive;
		alive.reserve(m_IDs->size());
		for (uint64_t id : *m_IDs)
		{
			if (IsParticleInWorldBounds(GetParticle(id)))
				alive.push_back(id);
			else
				RemoveParticle(id);   // NaN positions fail the bounds test too, so exploded particles get culled
		}
		*m_IDs = std::move(alive);
	}

	void HandleCollisions();
	void ApplySPH();

	void ApplyForces(DynamicSphere& particle, const std::vector<uint64_t>& neighbors);

	float GetDensity(const DynamicSphere& particle, const std::vector<uint64_t>& neighbors);

	void ApplyPreassureForce(DynamicSphere& particle, const std::vector<uint64_t>& neighbors);
	void ApplyExternalForce(DynamicSphere& particle, const std::vector<uint64_t>& neighbors);
	// TODO
	void ApplySurfaceForce(DynamicSphere& particle, const std::vector<uint64_t>& neighbors);
	void ApplyViscosityForce(DynamicSphere& particle, const std::vector<uint64_t>& neighbors);

	float Wpoly(float r) const
	{
		if (r < 0.0f || r >= H) return 0.0f;
		const float t = H * H - r * r;
		return POLY6 * t * t * t;
	}

	float Wspiky1stOrder(float r) const
	{
		if (r < 0.0f || r >= H) return 0.0f;
		const float t = H - r;
		return SPIKY * t * t;
	}

	// Laplacian of the viscosity kernel
	float Wviscosity2ndOrder(float r) const
	{
		if (r < 0.0f || r >= H) return 0.0f;
		return VISC * (H - r);
	}

private:
	std::unique_ptr<HeightGrid> m_TerrainGeometry = std::make_unique<HeightGrid>();
	std::shared_ptr<std::vector<uint64_t>> m_IDs = nullptr;
	std::shared_ptr<std::unordered_map<uint64_t, glm::vec3>> m_IDsToCenters = nullptr;
	std::unordered_map<uint64_t, DynamicSphere> m_IDsToParticles = std::unordered_map<uint64_t, DynamicSphere>();
	std::vector<std::vector<uint64_t>> m_Neighbors;
	std::vector<std::vector<std::vector<std::vector<uint64_t>>>> m_ParticleGrid = std::vector<std::vector<std::vector<std::vector<uint64_t>>>>();
	std::vector<CellIndex> m_OccupiedCells = std::vector<CellIndex>();
	uint32_t m_WorldLength = 10U;
	uint32_t m_WorldWidth  = 10U;
	uint32_t m_WorldDepth  = 10U;
	float m_ParticleSpacing = 0.5f;   // rest spacing; ~h/2 gives ~30 neighbors
	float m_RestDensity = 1000.0f;
	float m_Stiffness = 500.0f; // tune
	float m_Viscosity = 0.5f;   // tune
	float m_DeltaTime = 0.002f;
	const float POLY6 = 315.0f / (64.0f * PI * std::pow(H, 9.0f));
	const float SPIKY = -(45.0f / (PI * std::pow(H, 6.0f)));
	const float VISC = 45.0f / (PI * std::pow(H, 6.0f));
};
