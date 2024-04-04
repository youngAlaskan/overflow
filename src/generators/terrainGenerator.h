#pragma once

#include "../scene/terrain.h"
#include <FastNoise/FastNoise.h>

class TerrainGenerator {
public:
	TerrainGenerator() = default;

	std::vector<Vertex> GenerateVertices(FastNoise::SmartNode<FastNoise::Generator> generator);

private:
	void SetResX(uint32_t resX) { m_ResX = resX; }
	uint32_t GetResX() const { return m_ResX; }

	void SetResZ(uint32_t resZ) { m_ResZ = resZ; }
	uint32_t GetResZ() const { return m_ResZ; }

	void SetLength(float length) { m_Length = length; }
	float GetLength() const { return m_Length; }

	void SetWidth(float width) { m_Width = width; }
	float GetWidth() const { return m_Width; }

	void SetBaseElevation(float baseElevation) { m_BaseElevation = baseElevation; }
	float GetBaseElevation() const { return m_BaseElevation; }

	uint32_t m_ResX = 100U;
	uint32_t m_ResZ = 100U;
	float m_Width = 100.0f;
	float m_Length = 100.0f;
	float m_BaseElevation = -25.0f;
};