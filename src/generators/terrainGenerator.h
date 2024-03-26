#pragma once

#include "../scene/terrain.h"

class TerrainGenerator {
public:
	static GLfloat** GenerateHeightMap(unsigned int resX, unsigned int resY, float freq, unsigned int octaves);
	static std::vector<Vertex> GenerateVertices();
};