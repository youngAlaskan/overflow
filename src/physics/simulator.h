#pragma once

#include <memory>
#include <vector>

#include "primitives/sphere.h"
#include "primitives/heightfield.h"

inline static const glm::vec3 GRAVITY = glm::vec3(0.0f, -9.81f, 0.0f);

class Simulator
{
public:
	Simulator() { Init(); }
	Simulator(uint32_t width, uint32_t length, uint32_t depth) : m_WorldWidth(width), m_WorldLength(length), m_WorldDepth(depth) { Init(); }
	~Simulator() { CleanUp(); }

	void SetDeltaTime(float deltaTime) { m_DeltaTime = deltaTime; }
	float GetDeltaTime() const { return m_DeltaTime; }

	void SetWorldBounds(uint32_t width, uint32_t length)
	{
		m_WorldWidth = width;
		m_WorldLength = length;
	}

	void SetWorldWidth(uint32_t width) { m_WorldWidth = width; }
	uint32_t GetWorldWidth() const { return m_WorldWidth; }

	void SetWorldLength(uint32_t length) { m_WorldLength = length; }
	uint32_t SetWorldLength() const { return m_WorldLength; }

	void SetWorldDepth(uint32_t depth) { m_WorldDepth = depth; }
	uint32_t SetWorldDepth() const { return m_WorldDepth; }

	void RegisterParticle(const DynamicSphere& particle)
	{
		m_Particles.push_back(particle);
		GetParticleBlock(particle).push_back(particle);
	}

	void ClearParticles() { m_Particles.clear(); }
	void ClearParticleGrid() { m_ParticleGrid.clear(); }

	void Step();

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

	void CleanUp() {}

private:
	std::unique_ptr<Heightfield> m_TerrainGeometry = nullptr;
	std::vector<DynamicSphere> m_Particles = std::vector<DynamicSphere>();
	float m_DeltaTime = 0.0f;
	std::vector<std::vector<std::vector<std::vector<DynamicSphere>>>> m_ParticleGrid = std::vector<std::vector<std::vector<std::vector<DynamicSphere>>>>();
	uint32_t m_WorldLength = 0U;
	uint32_t m_WorldWidth  = 0U;
	uint32_t m_WorldDepth  = 0U;
};
