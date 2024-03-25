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
	Droplets(float radius = 1.0f) : m_Radius(radius)
	{
		SetSphereVBO();
		SetInstanceVBO({});
	}

	Droplets(std::vector<glm::vec3> centers, float radius = 1.0f) : m_Radius(radius)
	{
		SetSphereVBO();
		SetInstanceVBO(centers);
	}

	Droplets(const int subdivisions, std::vector<glm::vec3> centers, float radius = 1.0f)
		: m_Radius(radius), m_Subdivisions(subdivisions)
	{
		SetSphereVBO();
		SetInstanceVBO(centers);
	}

	void SetInstanceVBO(std::vector<glm::vec3> centers);
	void UpdateInstanceVBO() const;

	void UpdateVertexVBO(float radius);

public:
	std::vector<glm::vec3> m_Centers = std::vector<glm::vec3>();
	std::shared_ptr<VAO> m_VAO = std::make_shared<VAO>();
	VBO m_SphereVBO = VBO();
	VBO m_InstanceVBO = VBO();

	float m_Radius;

private:
	glm::vec3 SphereS(GLfloat u, GLfloat v) const;
	std::vector<glm::vec3> CreateSphere() const;

	void SetSphereVBO();

private:
	int m_Subdivisions = 5;
};
