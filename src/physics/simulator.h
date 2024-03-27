#pragma once

#include <memory>
#include <vector>

#include "primitives/sphere.h"
#include "primitives/heightMap.h"

class Simulator
{
public:
	Simulator() { Init(); }
	~Simulator() { CleanUp(); }

	void SetDeltaTime(float deltaTime) { m_DeltaTime = deltaTime; }
	float GetDeltaTime() const { return m_DeltaTime; }

	void Step();

private:
	void Init() {}

	void HandleCollisions();
	void ApplySPH();

	void CleanUp() {}

private:
	std::unique_ptr<Heightfield> m_TerrainGeometry = nullptr;
	std::vector<DynamicSphere> m_Particles = std::vector<DynamicSphere>();
	float m_DeltaTime = 0.0f;
};
