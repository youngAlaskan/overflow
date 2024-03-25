#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "../glObjects.h"

constexpr float TAU = 6.283185307179586f; // 2 * pi

class Droplets
{
public:
	Droplets()
	{
		SetSphereVBO();
		SetInstanceVBO({});
	}

	Droplets(std::vector<glm::vec3> centers)
	{
		SetSphereVBO();
		SetInstanceVBO(centers);
	}

	Droplets(const int subdivisions, std::vector<glm::vec3> centers)
		: m_Subdivisions(subdivisions)
	{
		SetSphereVBO();
		SetInstanceVBO(centers);
	}

	void SetInstanceVBO(std::vector<glm::vec3> centers);

public:
	std::vector<glm::vec3> m_Centers = std::vector<glm::vec3>();
	std::shared_ptr<VAO> m_VAO = std::make_shared<VAO>();
	VBO m_SphereVBO = VBO();
	VBO m_InstanceVBO = VBO();

private:
	std::vector<glm::vec3> CreateSphere();

	void SetSphereVBO();

private:
	int m_Subdivisions = 5;
};
