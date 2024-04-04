#include "terrainGenerator.h"

#include <FastNoise/FastNoise.h>
#include <iostream>
#include <cstdio>
#include <fstream>

std::vector<Vertex> TerrainGenerator::GenerateVertices(FastNoise::SmartNode<FastNoise::Generator> generator, unsigned int resX, unsigned int resZ, float sizeX, float sizeZ) { // TODO: Make heightmaps a parameter
	unsigned int seed = 1338;
	float freq = 0.005f;
	float heightMul = 10.0f;
	glm::vec4 offset = glm::vec4(0.0f, -25.0f, 0.0f, 0.0f);
	
	float* heightMap = new float[(resX + 1) * (resZ + 1)];
	generator->GenUniformGrid2D(heightMap, 0, 0, resX + 1, resZ + 1, freq, seed);
	auto og = glm::vec4(-sizeX * 0.5f, 0, -sizeZ * 0.5f, 1.0f) + offset;
	GLfloat dx = sizeX / (GLfloat)resX;
	GLfloat dz = sizeZ / (GLfloat)resZ;

	std::vector<Vertex> vertices = std::vector<Vertex>();

	auto img = std::ofstream("test.pgm");
	img << "P2\n" << resX << " " << resZ << "\n255\n";
	for (GLuint j = 0; j < resX; j++) {
		for (GLuint i = 0; i < resX; i++) {
			GLfloat x = i * dx;
			GLfloat z = j * dz;

			int idx = j * (resX + 1) + i;

			glm::vec4 a = glm::vec4(x, heightMap[idx] * heightMul , z, 0.0f);
			glm::vec4 b = glm::vec4(x + dx, heightMap[idx + 1] * heightMul, z, 0.0f);
			glm::vec4 c = glm::vec4(x, heightMap[idx + resX + 1] * heightMul, z + dz, 0.0f);
			glm::vec4 d = glm::vec4(x + dx, heightMap[idx + resX + 2] * heightMul, z + dz, 0.0f);

			auto norm = glm::cross(glm::vec3(c - a), glm::vec3(b - a));

			vertices.emplace_back(a + og, norm, glm::vec2(0.0f));
			vertices.emplace_back(c + og, norm, glm::vec2(0.0f));
			vertices.emplace_back(b + og, norm, glm::vec2(0.0f));

			norm = glm::cross(glm::vec3(d - b), glm::vec3(b - a));

			vertices.emplace_back(b + og, norm, glm::vec2(0.0f));
			vertices.emplace_back(c + og, norm, glm::vec2(0.0f));
			vertices.emplace_back(d + og, norm, glm::vec2(0.0f));
			img << std::min(256, std::max(0, static_cast<int>((heightMap[idx] * 0.5 + 0.5) * 255.99f))) << ' ';
		}
	}
	img.close();

	return vertices;
}

