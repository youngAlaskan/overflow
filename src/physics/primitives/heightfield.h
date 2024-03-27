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
	Heightfield(uint32_t length, uint32_t width) : m_Length(length), m_Width(width) {}

	void SetLength(uint32_t length) { m_Length = length; }
	uint32_t GetLength() const { return m_Length; }

	void SetWidth(uint32_t width) { m_Width = width; }
	uint32_t GetWidth() const { return m_Width; }

	void SetHeights(std::vector<std::vector<float>> heights) { m_Heights = heights; }
	std::vector<std::vector<float>> GetHeights() { return m_Heights; }

	float GetHeight(uint32_t s, uint32_t t) { return m_Heights[t][s]; }

	// Get the height at (s, t) using bilinear interpolation
	float GetHieght(float s, float t);

	glm::vec3 GetNormal(float s, float t);

private:
	uint32_t m_Length = 0U;
	uint32_t m_Width = 0U;
	std::vector<std::vector<float>> m_Heights = std::vector<std::vector<float>>();
};
