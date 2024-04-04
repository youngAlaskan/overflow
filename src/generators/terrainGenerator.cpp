#include "terrainGenerator.h"

#include <FastNoise/FastNoise.h>
#include <iostream>
#include <cstdio>
#include <fstream>

std::vector<Vertex> TerrainGenerator::GenerateVertices(FastNoise::SmartNode<FastNoise::Generator> generator) {
	unsigned int seed = 1338;
	float freq = 0.005f;
	float heightMul = 20.0f;
	
	float* heightMap = new float[(m_ResX + 1) * (m_ResZ + 1)];
	generator->GenUniformGrid2D(heightMap, 0, 0, m_ResX + 1, m_ResZ + 1, freq, seed);
	auto og = glm::vec4(-m_Width * 0.5f, m_BaseElevation, -m_Length * 0.5f, 1.0f);
	GLfloat dx = m_Width / (GLfloat)m_ResX;
	GLfloat dz = m_Length / (GLfloat)m_ResZ;

	std::vector<Vertex> vertices = std::vector<Vertex>();

	GLuint w = m_ResX + 1;

	for (GLuint j = 0; j < m_ResZ; j++) {
		for (GLuint i = 0; i < m_ResX; i++) {
			GLfloat x = i * dx;
			GLfloat z = j * dz;

			int idx = j * w + i;

			glm::vec4 a = glm::vec4(x, heightMap[idx] * heightMul, z, 0.0f);
			glm::vec4 b = glm::vec4(x + dx, heightMap[idx + 1] * heightMul, z, 0.0f);
			glm::vec4 c = glm::vec4(x, heightMap[idx + w] * heightMul, z + dz, 0.0f);
			glm::vec4 d = glm::vec4(x + dx, heightMap[idx + w + 1] * heightMul, z + dz, 0.0f);

			auto norm = glm::cross(glm::vec3(c - a), glm::vec3(b - a));

			vertices.emplace_back(a + og, norm, glm::vec2(0.0f));
			vertices.emplace_back(c + og, norm, glm::vec2(0.0f));
			vertices.emplace_back(b + og, norm, glm::vec2(0.0f));

			norm = glm::cross(glm::vec3(d - b), glm::vec3(b - a));

			vertices.emplace_back(b + og, norm, glm::vec2(0.0f));
			vertices.emplace_back(c + og, norm, glm::vec2(0.0f));
			vertices.emplace_back(d + og, norm, glm::vec2(0.0f));
		}
	}

	return vertices;
}

