#pragma once

#include "../scene/terrain.h"

class TerrainGenerator {
public:
	static GLfloat** GenerateHeightMap(unsigned int resX, unsigned int resY, float scale);
	static std::vector<Vertex> GenerateVertices();
};