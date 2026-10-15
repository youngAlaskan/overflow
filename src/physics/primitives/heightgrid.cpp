#include "heightgrid.h"

void HeightGrid::Build(const std::vector<glm::vec3>& verts)
{
	m_Heights = std::vector<float>();
	if (verts.size() < 4) return;

	float minX = verts[0].x, maxX = minX, minZ = verts[0].z, maxZ = minZ;
	std::vector<float> xs, zs;
	xs.reserve(verts.size()); zs.reserve(verts.size());
	for (const auto& v : verts)
	{
		minX = std::min(minX, v.x); maxX = std::max(maxX, v.x);
		minZ = std::min(minZ, v.z); maxZ = std::max(maxZ, v.z);
		xs.push_back(v.x); zs.push_back(v.z);
	}

	const int resX = CountDistinct(xs, 1e-4f * (maxX - minX));
	const int resZ = CountDistinct(zs, 1e-4f * (maxZ - minZ));
	if (resX < 2 || resZ < 2) return;

	m_MinX = minX; m_MaxX = maxX; m_MinZ = minZ; m_MaxZ = maxZ;
	m_ResX = resX; m_ResZ = resZ;
	m_Dx = (maxX - minX) / (resX - 1);
	m_Dz = (maxZ - minZ) / (resZ - 1);
	m_Heights.assign(size_t(resX) * resZ, 0.0f);

	for (const auto& v : verts)
	{
		const int ix = std::clamp(int(std::lround((v.x - minX) / m_Dx)), 0, resX - 1);
		const int iz = std::clamp(int(std::lround((v.z - minZ) / m_Dz)), 0, resZ - 1);
		m_Heights[size_t(iz) * resX + ix] = v.y;
	}
}

void HeightGrid::Eval(float x, float z, float& h, float& dhdx, float& dhdz) const
{
	const float gx = std::clamp((x - m_MinX) / m_Dx, 0.0f, float(m_ResX - 1));
	const float gz = std::clamp((z - m_MinZ) / m_Dz, 0.0f, float(m_ResZ - 1));
	const int ix = std::min(int(gx), m_ResX - 2);
	const int iz = std::min(int(gz), m_ResZ - 2);
	const float u = gx - ix, v = gz - iz;
	auto H = [&](int i, int j) { return m_Heights[size_t(j) * m_ResX + i]; };
	const float h00 = H(ix, iz), h10 = H(ix + 1, iz), h01 = H(ix, iz + 1), h11 = H(ix + 1, iz + 1);

	if (!m_AltDiagonal)   // diagonal from (0,0) to (1,1)
	{
		if (u >= v) { h = h00 + u * (h10 - h00) + v * (h11 - h10); dhdx = (h10 - h00) / m_Dx; dhdz = (h11 - h10) / m_Dz; }
		else { h = h00 + u * (h11 - h01) + v * (h01 - h00); dhdx = (h11 - h01) / m_Dx; dhdz = (h01 - h00) / m_Dz; }
	}
	else                  // diagonal from (1,0) to (0,1)
	{
		if (u + v <= 1.0f) { h = h00 + u * (h10 - h00) + v * (h01 - h00); dhdx = (h10 - h00) / m_Dx; dhdz = (h01 - h00) / m_Dz; }
		else { h = h11 + (1 - u) * (h01 - h11) + (1 - v) * (h10 - h11); dhdx = (h11 - h01) / m_Dx; dhdz = (h11 - h10) / m_Dz; }
	}
}
