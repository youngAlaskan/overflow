#include "simulator.h"
#include <omp.h>
#include <unordered_set>

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

			if (m_ParticleGrid[depthIndex][widthIndex][lengthIndex].empty())
			{
				m_OccupiedCells.push_back({ depthIndex, widthIndex, lengthIndex });
			}

			#pragma omp critical
			{
				m_ParticleGrid[depthIndex][widthIndex][lengthIndex].push_back(id);
			}
		}
	}
}

void Simulator::SetNeighbors()
{
	#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < m_IDs->size(); i++)
	{
		const auto& id = (*m_IDs)[i];
		std::set<uint64_t>& neighbors = GetParticleNeighbors(id);
		
		// Store the neighbors in the map if there are any, otherwise remove the entry for this particle
		#pragma omp critical
		{
			if (!neighbors.empty())
			{
				m_IDsToNeighbors[id] = neighbors;
			}
			else
			{
				m_IDsToNeighbors.erase(id);
			}
		}
	}
}

inline std::set<uint64_t> Simulator::GetParticleNeighbors(const uint64_t id)
{
	std::unordered_set<uint64_t> neighborSet;
	neighborSet.reserve(32);
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

					for (const auto& neighborID : block)
					{
						if (neighborID != id) // Exclude the particle itself
						{
							neighborSet.insert(neighborID);
						}
					}
				}
			}
		}
	}

	return std::set<uint64_t>(neighborSet.begin(), neighborSet.end());
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

	// Tunable boundary parameters
	const float contactDistanceFactor = 0.5f; // NOT the smoothing radius h; Later should be based on particle spacing
	const float restitution = 0.05f;		  // water barely bounces
	const float friction = 0.1f;			  // Coulomb coefficient, 0 = free-slip

	for (const auto& id : *m_IDs)
	{
		DynamicSphere& particle = GetParticle(id);
		glm::vec3 position = particle.GetPosition();

		// Outside the terrain footprint: nothing to collide with.
		if (position.x < minX || position.x > maxX || position.z < minZ || position.z > maxZ)
		{
			// Particle is outside the terrain footprint, remove it from the simulation
			RemoveParticle(id);
			continue;
		}

		// Handle Terrain collision
		const float height = m_TerrainGeometry->GetHeight(position.x, position.z);
		const glm::vec3 normal = glm::normalize(m_TerrainGeometry->GetNormal(position.x, position.z));

		// Distance to the surface measured along the normal (accounts for slope)
		const float distance = (position.y - height) * normal.y;
		const float penetration = contactDistanceFactor * particle.GetRadius() - distance;

		if (penetration <= 0.0f) continue; // No collision

		// 1. Position correction: push the particle back out along the normal
		position += normal * penetration;
		particle.SetPosition(position);

		// 2. Velocity response, only if the particle is moving into the surface
		glm::vec3 velocity = particle.GetVelocity();
		const float vn = glm::dot(velocity, normal);

		if (vn < 0.0f) // Particle is moving into the surface
		{
			const glm::vec3 vNormal = vn * normal;
			glm::vec3 vTangent = velocity - vNormal;

			// Coulomb friction: tangential speed lost is proportional to the normal impulse
			const float tangentSpeed = glm::length(vTangent);
			if (tangentSpeed > 1e-6f)
			{
				const float maxFrictionDrop = friction * (1.0f + restitution) * (-vn);
				const float newSpeed = glm::max(0.0f, tangentSpeed - maxFrictionDrop);
				vTangent *= newSpeed / tangentSpeed;
			}

			velocity = vTangent - restitution * vNormal;
			particle.SetVelocity(velocity);
		}
	}
}

void Simulator::ApplySPH()
{
	size_t limit = m_IDs->size();

	std::vector<std::vector<uint64_t>> neighbors = std::vector<std::vector<uint64_t>>(limit, std::vector<uint64_t>());

	// Set densities
	
	// Skip if there are no neighbors for any particle
	if (!m_IDsToNeighbors.empty())
	{
		#pragma omp parallel for
		for (int i = 0; i < limit; i++)
		{
			DynamicSphere& particle = GetParticle(m_IDs->at(i));

			// Skip if there are no neighbors for this particle
			if (m_IDsToNeighbors.find(m_IDs->at(i)) == m_IDsToNeighbors.end())
				continue;

			const auto& neighborIDs = m_IDsToNeighbors.at(m_IDs->at(i));
			for (const auto& neighborID : neighborIDs)
			{
				neighbors[i].push_back(neighborID);
			}
			particle.SetDensity(GetDensity(particle, neighbors[i]));
		}
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
		// Skip if the neighbor is not in the IDsToCenters map
		// Note: Neighbors that have fallen out of bounds may still be in the neighbor list, but they will not be in the IDsToCenters map
		if (m_IDsToCenters->find(neighborID) == m_IDsToCenters->end())
			continue;
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
