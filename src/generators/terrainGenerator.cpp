#include "terrainGenerator.h"
#include "noise.h"

#include <iostream>
#include <cstdio>
#include <fstream>

GLfloat** TerrainGenerator::GenerateHeightMap(unsigned int resX, unsigned int resY, float freq, unsigned int octaves) {
	GLfloat** heightMap = new GLfloat*[resY];

	for (GLuint j = 0; j < resY; j++) {
		heightMap[j] = new GLfloat[resX];
		for (GLuint i = 0; i < resX; i++) {
			GLfloat x = i * freq;
			GLfloat y = j * freq;
			heightMap[j][i] = 0;

			for (GLuint k = 0; k < octaves; k++) {
				GLfloat oct = (GLfloat)pow(2, k);
				heightMap[j][i] += (GLfloat)perlin(x * oct, y * oct) * pow(0.5, k);
			}
		}
	}

	return heightMap;
}

std::vector<Vertex> TerrainGenerator::GenerateVertices() { // TODO: Make heightmaps a parameter
	std::vector<Vertex> vertices = std::vector<Vertex>();

	GLfloat size_x = 100.0f;
	GLfloat size_z = 100.0f;

	GLuint subd_x = 100;
	GLuint subd_z = 100;

	GLfloat dx = size_x / (GLfloat)subd_x;
	GLfloat dz = size_z / (GLfloat)subd_z;

	GLfloat height = 1.0f;
	GLfloat** heightMap = GenerateHeightMap(subd_x + 1, subd_z + 1, 0.02f, 4);

	auto og = glm::vec4(-size_x / 2, -25.0f, -size_z / 2, 1.0f);

	auto img = std::ofstream("test.pgm");
	img << "P2\n" << subd_x + 1 << " " << subd_z + 1 << "\n255\n";
	for (GLuint j = 0; j < subd_z; j++) {
		for (GLuint i = 0; i < subd_x; i++) {
			GLfloat x = i * dx;
			GLfloat z = j * dz;

			glm::vec4 a = glm::vec4(x, height * heightMap[j][i] * height, z, 0.0f);
			glm::vec4 b = glm::vec4(x + dx, height * heightMap[j][i + 1] * height, z, 0.0f);
			glm::vec4 c = glm::vec4(x, height * heightMap[j + 1][i] * height, z + dz, 0.0f);
			glm::vec4 d = glm::vec4(x + dx, height * heightMap[j + 1][i + 1] * height, z + dz, 0.0f);

			auto norm = glm::cross(glm::vec3(c - a), glm::vec3(b - a));

			vertices.emplace_back(a + og, norm, glm::vec2(0.0f));
			vertices.emplace_back(c + og, norm, glm::vec2(0.0f));
			vertices.emplace_back(b + og, norm, glm::vec2(0.0f));

			norm = glm::cross(glm::vec3(d - b), glm::vec3(b - a));

			vertices.emplace_back(b + og, norm, glm::vec2(0.0f));
			vertices.emplace_back(c + og, norm, glm::vec2(0.0f));
			vertices.emplace_back(d + og, norm, glm::vec2(0.0f));

			img << static_cast<int>((heightMap[j][i] * 0.5 + 0.5) * 255.99f) << ' ';
		}
		delete heightMap[j];
	}
	img.close();
	delete heightMap;

	return vertices;
}

