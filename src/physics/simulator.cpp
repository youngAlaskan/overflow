#include "simulator.h"
#include <omp.h>
#include <unordered_set>

void Simulator::SetParticleGrid()
{
	for (uint64_t id : *m_IDs)
	{
		const DynamicSphere& p = GetParticle(id);
		auto& block = m_ParticleGrid[GetParticleDepthIndex(p)][GetParticleWidthIndex(p)][GetParticleLengthIndex(p)];
		if (block.empty())
			m_OccupiedCells.push_back({ GetParticleDepthIndex(p), GetParticleWidthIndex(p), GetParticleLengthIndex(p) });
		block.push_back(id);
	}
}

void Simulator::SetNeighbors()
{
	const int count = static_cast<int>(m_IDs->size());
	m_Neighbors.assign(count, {});

	#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < count; i++)
		m_Neighbors[i] = GetParticleNeighbors((*m_IDs)[i]);   // each thread writes its own slot, no critical needed
}

std::vector<uint64_t> Simulator::GetParticleNeighbors(const uint64_t id)
{
	std::vector<uint64_t> result;
	result.reserve(64);
	const DynamicSphere& particle = GetParticle(id);
	const uint32_t zi = GetParticleLengthIndex(particle);
	const uint32_t xi = GetParticleWidthIndex(particle);
	const uint32_t yi = GetParticleDepthIndex(particle);

	for (uint32_t y = yi ? yi - 1 : 0; y <= yi + 1 && y <= m_WorldDepth; y++)
		for (uint32_t x = xi ? xi - 1 : 0; x <= xi + 1 && x <= m_WorldWidth; x++)
			for (uint32_t z = zi ? zi - 1 : 0; z <= zi + 1 && z <= m_WorldLength; z++)
				for (uint64_t other : m_ParticleGrid[y][x][z])
					if (other != id) result.push_back(other);

	return result;
}

bool Simulator::IsParticleInWorldBounds(const DynamicSphere& particle) const
{
	const float maxX = static_cast<float>(m_WorldWidth) / 2.0f;
	const float minX = -maxX;
	const float maxZ = static_cast<float>(m_WorldLength) / 2.0f;
	const float minZ = -maxZ;
	const float maxY = static_cast<float>(m_WorldDepth) / 2.0f;
	const float minY = -maxY;

	glm::vec3 center = particle.GetPosition();
	return center.x >= minX && center.x <= maxX &&
		center.y >= minY && center.y <= maxY &&
		center.z >= minZ && center.z <= maxZ;
}

void Simulator::HandleCollisions()
{
	if (!m_TerrainGeometry || !m_IDs) return;

	// Tunable boundary parameters
	const float restitution = 0.05f;		  // water barely bounces
	const float friction = 0.1f;			  // Coulomb coefficient, 0 = free-slip

	for (const auto& id : *m_IDs)
	{
		DynamicSphere& particle = GetParticle(id);
		glm::vec3 position = particle.GetPosition();

		// Outside the terrain footprint: nothing to collide with.
		if (!m_TerrainGeometry->Contains(position.x, position.z)) continue;

		// Handle Terrain collision
		const float contactDistance = particle.GetRadius();   // matches the rendered radius
		const float height = m_TerrainGeometry->GetHeight(position.x, position.z);
		const glm::vec3 normal = glm::normalize(m_TerrainGeometry->GetNormal(position.x, position.z));

		// Distance to the surface measured along the normal (accounts for slope)
		const float distance = (position.y - height) * normal.y;
		const float penetration = contactDistance - distance;

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
	const int count = static_cast<int>(m_IDs->size());

	#pragma omp parallel for
	for (int i = 0; i < count; i++)
	{
		DynamicSphere& p = GetParticle((*m_IDs)[i]);
		p.SetDensity(GetDensity(p, m_Neighbors[i]));
	}

	#pragma omp parallel for
	for (int i = 0; i < count; i++)
		ApplyForces(GetParticle((*m_IDs)[i]), m_Neighbors[i]);

	#pragma omp parallel for
	for (int i = 0; i < count; i++)
	{
		DynamicSphere& p = GetParticle((*m_IDs)[i]);
		p.Update(m_DeltaTime);
		p.ClearForces();
	}

	HandleCollisions();

	if (m_IDsToCenters)
		for (uint64_t id : *m_IDs)
			(*m_IDsToCenters)[id] = GetParticle(id).GetPosition();

	CullOutOfBounds();   // removes dead particles and rebuilds m_IDs
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
	float sum = Wpoly(0.0f); // self contribution
	for (uint64_t neighborID : neighborIDs)
		sum += Wpoly(glm::length(particle.GetPosition() - GetParticle(neighborID).GetPosition()));
	return particle.GetMass() * sum;
}

void Simulator::ApplyPreassureForce(DynamicSphere& particle, const std::vector<uint64_t>& neighborIDs)
{
	const float pi = glm::max(0.0f, m_Stiffness * (particle.GetDensity() - m_RestDensity));

	glm::vec3 fDensity(0.0f);
	for (uint64_t neighborID : neighborIDs)
	{
		const DynamicSphere& n = GetParticle(neighborID);
		glm::vec3 r = particle.GetPosition() - n.GetPosition();
		float dist = glm::length(r);
		if (dist < 1e-6f || dist >= H) continue;

		float pj = glm::max(0.0f, m_Stiffness * (n.GetDensity() - m_RestDensity));
		fDensity -= n.GetMass() * (pi + pj) / (2.0f * n.GetDensity())
			* Wspiky1stOrder(dist) * (r / dist);
	}
	particle.ApplyForce(fDensity * (particle.GetMass() / particle.GetDensity())); // force density -> force
}

void Simulator::ApplyExternalForce(DynamicSphere& particle, const std::vector<uint64_t>& neighbors)
{
	particle.ApplyForce(particle.GetMass() * GRAVITY);
}

// TODO
void Simulator::ApplySurfaceForce(DynamicSphere& particle, const std::vector<uint64_t>& neighborIDs)
{
}

void Simulator::ApplyViscosityForce(DynamicSphere& particle, const std::vector<uint64_t>& neighborIDs)
{
	glm::vec3 fDensity(0.0f);
	for (uint64_t neighborID : neighborIDs)
	{
		const DynamicSphere& n = GetParticle(neighborID);
		float dist = glm::length(particle.GetPosition() - n.GetPosition());
		if (dist >= H) continue;
		fDensity += n.GetMass() * (n.GetVelocity() - particle.GetVelocity()) / n.GetDensity()
			* Wviscosity2ndOrder(dist);
	}
	particle.ApplyForce(m_Viscosity * fDensity * (particle.GetMass() / particle.GetDensity()));
}
