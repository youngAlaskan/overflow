#pragma once

#include "../scene/terrain.h"
#include <FastNoise/FastNoise.h>

class TerrainGenerator {
public:
	static std::vector<Vertex> GenerateVertices(FastNoise::SmartNode<FastNoise::Generator> generator, unsigned int resX, unsigned int resZ, float sizeX, float sizeZ);
};