#include "heightfield.h"

// Get the height at (s, t) using bilinear interpolation
float Heightfield::GetHeight(float x, float z)
{
	float s = x + static_cast<float>(m_Width / 2U);
	float t = z + static_cast<float>(m_Length / 2U);
	uint32_t sLow = static_cast<uint32_t>(s);
	if (sLow >= m_Width) sLow = m_Width - 1U;
	float sOffset = s - static_cast<float>(sLow);
	uint32_t tLow = static_cast<uint32_t>(t);
	if (tLow >= m_Length) tLow = m_Length - 1U;
	float tOffset = t - static_cast<float>(tLow);

	// Calculate the height values at the four neighboring points
	float TL, TR, BR, BL;

	if (sLow == m_Width - 1U && tLow == m_Length - 1U)
	{
		return GetHeight(sLow, tLow);
	}
	else if (sLow == m_Width - 1U)
	{
		TL = m_Heights[tLow][sLow];
		TR = m_Heights[tLow][sLow];
		BR = m_Heights[static_cast<uint64_t>(tLow) + 1ULL][sLow];
		BL = m_Heights[static_cast<uint64_t>(tLow) + 1ULL][sLow];
	}
	else if (tLow == m_Length - 1U)
	{
		TL = m_Heights[tLow][sLow];
		TR = m_Heights[tLow][static_cast<uint64_t>(sLow) + 1ULL];
		BR = m_Heights[tLow][static_cast<uint64_t>(sLow) + 1ULL];
		BL = m_Heights[tLow][sLow];
	}
	else
	{
		TL = m_Heights[tLow][sLow];
		TR = m_Heights[tLow][static_cast<uint64_t>(sLow) + 1ULL];
		BR = m_Heights[static_cast<uint64_t>(tLow) + 1ULL][static_cast<uint64_t>(sLow) + 1ULL];
		BL = m_Heights[static_cast<uint64_t>(tLow) + 1ULL][sLow];
	}

	float leftLerp = TL + tOffset * (BL - TL);
	float rightLerp = TR + tOffset * (BR - TR);

	return leftLerp + sOffset * (rightLerp - leftLerp);
}

glm::vec3 Heightfield::GetNormal(float x, float z)
{
	float s = x + static_cast<float>(m_Width / 2U);
	float t = z + static_cast<float>(m_Length / 2U);

	uint32_t sLow = static_cast<uint32_t>(s);
	if (sLow >= m_Width) sLow = m_Width - 1U;
	float sOffset = s - static_cast<float>(sLow);
	uint32_t tLow = static_cast<uint32_t>(t);
	if (tLow >= m_Length) tLow = m_Length - 1U;
	float tOffset = t- static_cast<float>(tLow);

	// Calculate the height values at three of the neighboring points
	float TL, TR, BL;

	if (sLow == m_Width - 1U && tLow == m_Length - 1U)
	{
		TL = TR = BL = m_Heights[tLow][sLow];
	}
	else if (sLow == m_Width - 1U)
	{
		TL = m_Heights[tLow][sLow];
		TR = m_Heights[tLow][sLow];
		BL = m_Heights[static_cast<uint64_t>(tLow) + 1ULL][sLow];
	}
	else if (tLow == m_Length - 1U)
	{
		TL = m_Heights[tLow][sLow];
		TR = m_Heights[tLow][static_cast<uint64_t>(sLow) + 1ULL];
		BL = m_Heights[tLow][sLow];
	}
	else
	{
		TL = m_Heights[tLow][sLow];
		TR = m_Heights[tLow][static_cast<uint64_t>(sLow) + 1ULL];
		BL = m_Heights[static_cast<uint64_t>(tLow) + 1ULL][sLow];
	}

	// Calculate the gradient in the s and t directions
	glm::vec3 gradientS = glm::vec3(1.0f, TR - TL, 0.0f);
	glm::vec3 gradientT = glm::vec3(0.0f, BL - TL, 1.0f);

	// Calculate the normal using the gradient
	glm::vec3 normal = glm::normalize(glm::cross(gradientS, gradientT));
	
	// Ensure normal is facing upwards
	if (normal.y < 0.0f) normal = -1.0f * normal;

	return normal;
}
