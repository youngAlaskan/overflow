#include "heightfield.h"

// Get the height at (s, t) using bilinear interpolation
float Heightfield::GetHieght(float s, float t)
{
	uint32_t sLow = static_cast<uint32_t>(s);
	float sOffset = s - static_cast<float>(sLow);
	uint32_t tLow = static_cast<uint32_t>(t);
	float tOffset = t - static_cast<float>(tLow);

	// Calculate the height values at the four neighboring points
	float TL, TR, BR, BL;

	if (sLow == m_Width - 1U && tLow == m_Length - 1U)
	{
		return GetHeight(sLow, tLow);
	}
	else if (sLow == m_Width - 1U)
	{
		TL = m_Heights[t][s];
		TR = m_Heights[t][s];
		BR = m_Heights[t + 1U][s];
		BL = m_Heights[t + 1U][s];
	}
	else if (tLow == m_Length - 1U)
	{
		TL = m_Heights[t][s];
		TR = m_Heights[t][s + 1U];
		BR = m_Heights[t][s + 1U];
		BL = m_Heights[t][s];
	}
	else
	{
		TL = m_Heights[t][s];
		TR = m_Heights[t][s + 1U];
		BR = m_Heights[t + 1U][s + 1U];
		BL = m_Heights[t + 1U][s];
	}

	float leftLerp = TL + tOffset * (BL - TL);
	float rightLerp = TR + tOffset * (BR - TR);

	return leftLerp + sOffset * (rightLerp - leftLerp);
}

glm::vec3 Heightfield::GetNormal(float s, float t)
{
	uint32_t sLow = static_cast<uint32_t>(s);
	float sOffset = s - static_cast<float>(sLow);
	uint32_t tLow = static_cast<uint32_t>(t);
	float tOffset = t - static_cast<float>(tLow);

	// Calculate the height values at three of the neighboring points
	float TL, TR, BL;

	if (sLow == m_Width - 1U && tLow == m_Length - 1U)
	{
		TL = TR = BL = m_Heights[t][s];
	}
	else if (sLow == m_Width - 1U)
	{
		TL = m_Heights[t][s];
		TR = m_Heights[t][s];
		BL = m_Heights[t + 1U][s];
	}
	else if (tLow == m_Length - 1U)
	{
		TL = m_Heights[t][s];
		TR = m_Heights[t][s + 1U];
		BL = m_Heights[t][s];
	}
	else
	{
		TL = m_Heights[t][s];
		TR = m_Heights[t][s + 1U];
		BL = m_Heights[t + 1U][s];
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
