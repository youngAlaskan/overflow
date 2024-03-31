#include "simulator.h"

inline std::vector<DynamicSphere> Simulator::GetParticleNeighbors(const DynamicSphere& particle)
{
	std::vector<DynamicSphere> neighbors = std::vector<DynamicSphere>();
	uint32_t lengthIndex = GetParticleLengthIndex(particle);
	uint32_t widthIndex = GetParticleWidthIndex(particle);
	uint32_t depthIndex = GetParticleDepthIndex(particle);

	for (uint32_t y = depthIndex >= 1U ? depthIndex - 1 : depthIndex; y <= depthIndex + 1; y++) {
		for (uint32_t x = widthIndex >= 1U ? widthIndex - 1 : widthIndex; x <= widthIndex + 1; x++) {
			for (uint32_t z = lengthIndex >= 1U ? lengthIndex - 1 : lengthIndex; z <= lengthIndex + 1; z++) {
				if (z < m_WorldLength && x < m_WorldWidth && y < m_WorldDepth) {
					std::vector<DynamicSphere>& block = m_ParticleGrid[z][x][y];
					if (block.empty()) continue;
					neighbors.insert(neighbors.end(), block.begin(), block.end()); // Append vector
				}
			}
		}
	}

	auto iter = find(neighbors.begin(), neighbors.end(), particle);
	if (iter != neighbors.end())
		neighbors.erase(iter);

	return neighbors;
}

void Simulator::HandleCollisions()
{
	if (!m_TerrainGeometry || !m_IDs) return;

	const float maxX = static_cast<float>(m_TerrainGeometry->GetWidth() / 2U);
	const float minX = -maxX;
	const float maxZ = static_cast<float>(m_TerrainGeometry->GetLength() / 2U);
	const float minZ = -maxZ;

	for (const auto& id : *m_IDs)
	{
		DynamicSphere particle = GetParticle(id);
		glm::vec3 center = particle.GetPosition();
		// Check that center is within height field bounds
		if (center.x >= minX && center.x <= maxX && center.z >= minZ && center.z <= maxZ)
		{
			float offset = abs(m_TerrainGeometry->GetHeight(GetParticleWidthIndex(center.x), GetParticleLengthIndex(center.z)) - center.y);
			// Check that sphere is intersecting height field
			if (offset < particle.GetRadius())
				particle.SetPosition(center + m_TerrainGeometry->GetNormal(center.x, center.z) * offset); // Push sphere outwards
		}
	}
}

void Simulator::ApplySPH()
{
	std::vector<std::vector<DynamicSphere>> neighbors = std::vector<std::vector<DynamicSphere>>(m_IDs->size());

	// Set densities
	for (uint32_t i = 0; i < m_IDs->size(); i++)
	{
		DynamicSphere particle = GetParticle(m_IDs->at(i));
		neighbors[i] = GetParticleNeighbors(particle);
		particle.SetDensity(GetDensity(particle, neighbors[i]));
	}

	// Update based on calculated forces
	for (uint32_t i = 0; i < m_IDs->size(); i++)
	{
		DynamicSphere particle = GetParticle(m_IDs->at(i));
		ApplyForces(particle, neighbors[i]);
		particle.Update(m_DeltaTime);
		particle.ClearForces();
	}
}

void Simulator::ApplyForces(DynamicSphere& particle, const std::vector<DynamicSphere>& neighbors)
{
	ApplyPreassureForce(particle, neighbors);
	ApplyExternalForce(particle, neighbors);
	ApplySurfaceForce(particle, neighbors);
	ApplyViscosityForce(particle, neighbors);
}

float Simulator::GetDensity(const DynamicSphere& particle, const std::vector<DynamicSphere>& neighbors)
{
	if (neighbors.empty())
		return 1.0f;

	float density = 0.0f;

	for (const auto& neighbor : neighbors)
	{
		density += Wpoly(glm::length(particle.GetPosition() - neighbor.GetPosition()));
	}

	density *= particle.GetMass();

	return density;
}

void Simulator::ApplyPreassureForce(DynamicSphere& particle, const std::vector<DynamicSphere>& neighbors)
{
	glm::vec3 force = glm::vec3(0.0f);

	static double gasConstant = 0.0; // TODO
	static double restDensity = 1000;

	for (const auto& neighbor : neighbors)
	{
		force += (neighbor.GetDensity() - 2 * restDensity + particle.GetDensity()) / (2 * neighbor.GetDensity()) * Wspiky1stOrder(glm::length(particle.GetPosition() - neighbor.GetPosition()));
	}

	force *= -particle.GetMass() * gasConstant;

	particle.ApplyForce(force);
}

void Simulator::ApplyExternalForce(DynamicSphere& particle, const std::vector<DynamicSphere>& neighbors)
{
	particle.ApplyForce(particle.GetDensity() * GRAVITY);
}

// TODO
void Simulator::ApplySurfaceForce(DynamicSphere& particle, const std::vector<DynamicSphere>& neighbors)
{
	glm::vec3 force = glm::vec3(0.0f);



	particle.ApplyForce(force);
}

void Simulator::ApplyViscosityForce(DynamicSphere& particle, const std::vector<DynamicSphere>& neighbors)
{
	glm::vec3 force = glm::vec3(0.0f);

	static constexpr float viscosity = 55.0f;

	const glm::vec3 velocity = particle.GetVelocity();

	for (const auto& neighbor : neighbors)
	{
		force += (neighbor.GetVelocity() - velocity) / neighbor.GetDensity() * Wviscosity2ndOrder(glm::length(particle.GetPosition() - neighbor.GetPosition()));
	}

	force *= particle.GetMass() * viscosity;

	particle.ApplyForce(force);
}
