#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <vector>
#include <assert.h>

class Heightfield
{
public:
	Heightfield() = default;
	Heightfield(uint32_t dimension) : m_Length(dimension), m_Width(dimension) {}
	Heightfield(uint32_t length, uint32_t width) : m_Length(length), m_Width(width) { m_Heights = std::vector<std::vector<float>>(m_Length, std::vector<float>(m_Width)); }
	Heightfield(uint32_t length, uint32_t width, float height) : m_Length(length), m_Width(width) { m_Heights = std::vector<std::vector<float>>(m_Length, std::vector<float>(m_Width, height)); }

	void SetHeightsFromTriangleMesh(const std::vector<glm::vec3>& vertices)
	{
		for (const auto& vertex : vertices)
		{
			const uint32_t x = static_cast<uint32_t>(vertex.x + static_cast<float>(m_Width) / 2.0f);
			const uint32_t z = static_cast<uint32_t>(vertex.z + static_cast<float>(m_Length) / 2.0f);
			SetHeight(x, z, vertex.y);
		}
	}

	void SetLength(uint32_t length) { m_Length = length; }
	uint32_t GetLength() const { return m_Length; }

	void SetWidth(uint32_t width) { m_Width = width; }
	uint32_t GetWidth() const { return m_Width; }

	void SetHeights(std::vector<std::vector<float>> heights) { m_Heights = heights; }
	std::vector<std::vector<float>> GetHeights() const { return m_Heights; }

	void SetHeight(const uint32_t s, const uint32_t t, const float height) { m_Heights[t][s] = height; }
	float GetHeight(uint32_t s, uint32_t t) const { return m_Heights[t][s]; }

	void SetHeight(float x, float z, const float height);
	// Get the height at (s, t) using bilinear interpolation
	float GetHeight(float x, float z) const;

	glm::vec3 GetNormal(float s, float t) const;

	void Copy(const Heightfield& other)
	{
		for (uint32_t i = 0; i < m_Length; i++)
		{
			for (uint32_t j = 0; j < m_Width; j++)
			{
				SetHeight(j, i, other.GetHeight(j, i));
			}
		}
	}

	void Clear()
	{
		for (uint32_t i = 0; i < m_Length; i++)
		{
			for (uint32_t j = 0; j < m_Width; j++)
			{
				SetHeight(j, i, 0.0f);
			}
		}
	}

private:
	uint32_t m_Length = 0U;
	uint32_t m_Width = 0U;
	std::vector<std::vector<float>> m_Heights = std::vector<std::vector<float>>();
};
