#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>

class HeightGrid
{
public:
	void Build(const std::vector<glm::vec3>& verts);

	bool Valid() const { return !m_Heights.empty(); }
	bool Contains(float x, float z) const { return x >= m_MinX && x <= m_MaxX && z >= m_MinZ && z <= m_MaxZ; }
	float GetWidth() const { return m_MaxX - m_MinX; }
	float GetLength() const { return m_MaxZ - m_MinZ; }
	void SetAltDiagonal(bool alt) { m_AltDiagonal = alt; }

	float GetHeight(float x, float z) const
	{
		float h, dx, dz;
		Eval(x, z, h, dx, dz);
		return h;
	}

	glm::vec3 GetNormal(float x, float z) const
	{
		float h, dhdx, dhdz;
		Eval(x, z, h, dhdx, dhdz);
		return glm::normalize(glm::vec3(-dhdx, 1.0f, -dhdz));
	}

private:
	static int CountDistinct(std::vector<float> c, float eps)
	{
		std::sort(c.begin(), c.end());
		int n = 1;
		for (size_t i = 1; i < c.size(); i++)
			if (c[i] - c[i - 1] > eps) n++;
		return n;
	}

	void Eval(float x, float z, float& h, float& dhdx, float& dhdz) const;

private:
	std::vector<float> m_Heights = std::vector<float>();
	int m_ResX = 0, m_ResZ = 0;
	float m_MinX = 0, m_MaxX = 0, m_MinZ = 0, m_MaxZ = 0, m_Dx = 1, m_Dz = 1;
	bool m_AltDiagonal = true;
};