#include "simulator.h"

void Simulator::HandleCollisions()
{
	if (!m_TerrainGeometry) return;

	const float maxX = static_cast<float>(m_TerrainGeometry->GetWidth() / 2U);
	const float minX = -maxX;
	const float maxZ = static_cast<float>(m_TerrainGeometry->GetLength() / 2U);
	const float minZ = -maxZ;

	for (auto& particle : m_Particles)
	{
		glm::vec3 center = particle.GetCenter();
		// Check that center is within height field bounds
		if (center.x >= minX && center.x <= maxX && center.z >= minZ && center.z <= maxZ)
		{
			float offset = abs(m_TerrainGeometry->GetHeight(center.x, center.z) - center.y);
			// Check that sphere is intersecting height field
			if (offset < particle.GetRadius())
				particle.SetCenter(center + m_TerrainGeometry->GetNormal(center.x, center.z) * offset); // Push sphere outwards
		}
	}
}

void Simulator::ApplySPH()
{

}
