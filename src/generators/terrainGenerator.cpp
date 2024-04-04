#include "terrainGenerator.h"

#include <iostream>
#include <cstdio>
#include <fstream>

std::vector<Vertex> TerrainGenerator::GenerateVertices(FastNoise::SmartNode<FastNoise::Generator> generator) {	
	GLuint hmWidth = m_ResX + 2;
	GLuint hmLength = m_ResZ + 2;
	float* heightMap = new float[hmWidth * hmLength];
	generator->GenUniformGrid2D(heightMap, 0, 0, hmWidth, hmLength, m_Freq, m_Seed);
	auto og = glm::vec4(-m_Width * 0.5f, m_BaseElevation, -m_Length * 0.5f, 1.0f);
	GLfloat dx = m_Width / (GLfloat)m_ResX;
	GLfloat dz = m_Length / (GLfloat)m_ResZ;

	std::vector<Vertex> vertices = std::vector<Vertex>();

	// Precalculate vertex values
	Vertex* vertexMap = new Vertex[m_ResX * m_ResZ];
	for (GLuint j = 0; j < m_ResZ; j++) {
		for (GLuint i = 0; i < m_ResX; i++) {
			int vIdx = i + m_ResX * j;
			int hIdx = 1 + i + hmWidth * (1 + j);
			GLfloat x = i * dx;
			GLfloat z = j * dz;

			vertexMap[vIdx].Position = glm::vec4(x, heightMap[hIdx] * m_HeightMul, z, 0.0f) + og;
			vertexMap[vIdx].Normal = glm::normalize(
				glm::vec3(2.0 * (heightMap[hIdx + 1] - heightMap[hIdx - 1]) * m_HeightMul, 
					2.0 * (heightMap[hIdx + m_ResX] - heightMap[hIdx - m_ResX]) * m_HeightMul, -4.f)
			);
			vertexMap[vIdx].TexCoord = glm::vec2(0.f);
		}
	}

	// Build mesh
	for (GLuint j = 0; j < m_ResZ - 1; j++) {
		for (GLuint i = 0; i < m_ResX - 1; i++) {
			int idx = j * m_ResX + i;

			auto a = vertexMap[idx];
			auto b = vertexMap[idx + 1];
			auto c = vertexMap[idx + m_ResX];
			auto d = vertexMap[idx + m_ResX + 1];

			vertices.push_back(a);
			vertices.push_back(c);
			vertices.push_back(b);
			vertices.push_back(b);
			vertices.push_back(c);
			vertices.push_back(d);
		}
	}

	return vertices;
}

