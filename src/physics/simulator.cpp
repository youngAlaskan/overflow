#include "simulator.h"
#include <omp.h>

void Simulator::SetParticleGrid()
{
	std::vector<std::vector<uint64_t>> particleIDs(m_IDs->size());

	#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < m_IDs->size(); i++)
	{
		const auto& id = (*m_IDs)[i];
		DynamicSphere& particle = GetParticle(id);
		uint32_t lengthIndex = GetParticleLengthIndex(particle);
		uint32_t widthIndex = GetParticleWidthIndex(particle);
		uint32_t depthIndex = GetParticleDepthIndex(particle);

		#pragma omp critical
		{
			particleIDs[i].push_back(id);
		}
	}

	// Merge the local vectors into m_ParticleGrid
	for (size_t i = 0; i < particleIDs.size(); i++)
	{
		for (const auto& id : particleIDs[i])
		{
			uint32_t lengthIndex = GetParticleLengthIndex(GetParticle(id));
			uint32_t widthIndex = GetParticleWidthIndex(GetParticle(id));
			uint32_t depthIndex = GetParticleDepthIndex(GetParticle(id));

			#pragma omp critical
			{
				m_ParticleGrid[depthIndex][widthIndex][lengthIndex].push_back(id);
			}
		}
	}
}

void Simulator::SetNeighbors()
{
	for (int i = 0; i < m_IDs->size(); i++)
	{
		const auto& id = (*m_IDs)[i];
		m_IDsToNeighbors[id] = GetParticleNeighbors(id);
	}
}

inline std::vector<uint64_t> Simulator::GetParticleNeighbors(const uint64_t id)
{
	std::vector<uint64_t> neighbors = std::vector<uint64_t>();
	DynamicSphere& particle = GetParticle(id);
	uint32_t lengthIndex = GetParticleLengthIndex(particle);
	uint32_t widthIndex = GetParticleWidthIndex(particle);
	uint32_t depthIndex = GetParticleDepthIndex(particle);

	for (uint32_t y = depthIndex >= 1U ? depthIndex - 1 : depthIndex; y <= depthIndex + 1; y++) {
		for (uint32_t x = widthIndex >= 1U ? widthIndex - 1 : widthIndex; x <= widthIndex + 1; x++) {
			for (uint32_t z = lengthIndex >= 1U ? lengthIndex - 1 : lengthIndex; z <= lengthIndex + 1; z++) {
				if (z < m_WorldLength && x < m_WorldWidth && y < m_WorldDepth) {
					std::vector<uint64_t>& block = m_ParticleGrid[y][x][z];
					if (block.empty()) continue;
					neighbors.insert(neighbors.end(), block.begin(), block.end()); // Append vector
				}
			}
		}
	}

	auto iter = find(neighbors.begin(), neighbors.end(), id);
	if (iter != neighbors.end())
		neighbors.erase(iter);

	return neighbors;
}

inline bool Simulator::IsParticleInWorldBounds(const DynamicSphere& particle) const
{
	const float maxX = static_cast<float>(m_WorldWidth / 2U);
	const float minX = -maxX;
	const float maxZ = static_cast<float>(m_WorldLength / 2U);
	const float minZ = -maxZ;
	const float maxY = static_cast<float>(m_WorldDepth / 2U);
	const float minY = -maxY;

	glm::vec3 center = particle.GetPosition();
	return center.x >= minX && center.x <= maxX &&
		center.y >= minY && center.y <= maxY &&
		center.z >= minZ && center.z <= maxZ;
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
		DynamicSphere& particle = GetParticle(id);
		glm::vec3 center = particle.GetPosition();

		// Handle Terrain collision
		if (center.x >= minX && center.x <= maxX && center.z >= minZ && center.z <= maxZ)
		{
			// Get vertical offset from terrain
			float offset = center.y - m_TerrainGeometry->GetHeight(center.x, center.z);

			// Check if sphere is intersecting height field
			if (particle.GetRadius() >= offset)
			{
				particle.SetPosition(center + glm::vec3(0.0f, particle.GetRadius() - offset, 0.0f)); // Push sphere outwards
				glm::vec3 normal = m_TerrainGeometry->GetNormal(center.x, center.z);
				glm::vec3 incoming = particle.GetVelocity();
				glm::vec3 projection = glm::dot(incoming, normal) / glm::length(normal) * normal;
				glm::vec3 perp = incoming - projection;
				particle.SetVelocity(perp * 0.9f);
			}
		}

		float minDistance = 2.0f * particle.GetRadius();

		// Handle sphere-sphere collision
		const std::vector<uint64_t>& neighborIDs = m_IDsToNeighbors.at(id);
		for (const auto neighborID : neighborIDs)
		{
			DynamicSphere& neighbor = GetParticle(neighborID);
			glm::vec3 toNeighbor = neighbor.GetPosition() - center;
			glm::vec3 toParticle = -1.0f * toNeighbor;
			if (glm::length(toNeighbor) <= minDistance)
			{
				particle.SetPosition(particle.GetPosition() + toParticle * (minDistance - glm::length(toParticle)) * 0.5f);
				neighbor.SetPosition(neighbor.GetPosition() + toNeighbor * (minDistance - glm::length(toNeighbor)) * 0.5f);
			}
		}

	}
}

void Simulator::ApplySPH()
{
	size_t limit = m_IDs->size();

	std::vector<std::vector<uint64_t>> neighbors = std::vector<std::vector<uint64_t>>(limit, std::vector<uint64_t>());

	// Set densities
	#pragma omp parallel for
	for (int i = 0; i < limit; i++)
	{
		DynamicSphere& particle = GetParticle(m_IDs->at(i));
		const auto& neighborIDs = m_IDsToNeighbors.at(m_IDs->at(i));
		for (const auto& neighborID : neighborIDs)
		{
			neighbors[i].push_back(neighborID);
		}
		particle.SetDensity(GetDensity(particle, neighbors[i]));
	}

	std::vector<uint64_t> deadIDs = std::vector<uint64_t>();

	// Update based on calculated forces
	#pragma omp parallel for
	for (int i = 0; i < limit; i++)
	{
		DynamicSphere& particle = GetParticle(m_IDs->at(i));

		ApplyForces(particle, neighbors[i]);
		particle.Update(m_DeltaTime);
		if (IsParticleInWorldBounds(particle))
		{
			#pragma omp critical
			{
				particle.ClearForces();
				m_IDsToCenters->at(m_IDs->at(i)) = particle.GetPosition();
			}
		}
		else
		{
			#pragma omp critical
			{
				RemoveParticle(m_IDs->at(i));
				deadIDs.push_back(m_IDs->at(i));
			}
		}
	}

	for (auto id : deadIDs)
	{
		auto iter = find(m_IDs->begin(), m_IDs->end(), id);
		if (iter != m_IDs->end())
			m_IDs->erase(iter);
	}
}

void Simulator::ApplyForces(DynamicSphere& particle, const std::vector<uint64_t>& neighbors)
{
	ApplyPreassureForce(particle, neighbors);
	ApplyExternalForce(particle, neighbors);
	ApplySurfaceForce(particle, neighbors);
	ApplyViscosityForce(particle, neighbors);
}

float Simulator::GetDensity(const DynamicSphere& particle, const std::vector<uint64_t>& neighborIDs)
{
	float density = 0.0f;

	for (const auto& neighborID : neighborIDs)
	{
		glm::vec3 toParticle = particle.GetPosition() - m_IDsToCenters->at(neighborID);
		float distance = glm::length(toParticle);
		density += Wpoly(distance);
	}

	density *= particle.GetMass();

	return density == 0.0f ? 1.0f : density;
}

void Simulator::ApplyPreassureForce(DynamicSphere& particle, const std::vector<uint64_t>& neighborIDs)
{
	glm::vec3 force = glm::vec3(0.0f);

	static double gasConstant = 0.0; // TODO
	static double restDensity = 1000;

	for (const auto& neighborID : neighborIDs)
	{
		DynamicSphere& neighbor = GetParticle(neighborID);
		force += (neighbor.GetDensity() - 2 * restDensity + particle.GetDensity()) / (2 * neighbor.GetDensity()) * Wspiky1stOrder(glm::length(particle.GetPosition() - neighbor.GetPosition()));
	}

	force *= -particle.GetMass() * gasConstant;

	particle.ApplyForce(force);
}

void Simulator::ApplyExternalForce(DynamicSphere& particle, const std::vector<uint64_t>& neighbors)
{
	particle.ApplyForce(particle.GetDensity() * GRAVITY);
}

// TODO
void Simulator::ApplySurfaceForce(DynamicSphere& particle, const std::vector<uint64_t>& neighborIDs)
{
}

void Simulator::ApplyViscosityForce(DynamicSphere& particle, const std::vector<uint64_t>& neighborIDs)
{
	glm::vec3 force = glm::vec3(0.0f);

	static constexpr float viscosity = 55.0f;

	const glm::vec3 velocity = particle.GetVelocity();

	for (const auto& neighborID : neighborIDs)
	{
		DynamicSphere& neighbor = GetParticle(neighborID);
		force += (neighbor.GetVelocity() - velocity) / neighbor.GetDensity() * Wviscosity2ndOrder(glm::length(particle.GetPosition() - neighbor.GetPosition()));
	}

	force *= particle.GetMass() * viscosity;

	particle.ApplyForce(force);
}
