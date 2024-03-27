#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "../glObjects.h"

constexpr float TAU = 6.283185307179586f; // 2 * pi

class Droplet
{
public:
	Droplet() : m_Position(0.0f) {}

	void SetPosition(const glm::vec3& position) { m_Position = position; }
	glm::vec3 GetPosition() const { return m_Position; }

	static void SetMass(const float mass) { m_Mass = mass; }
	static float GetMass() { return m_Mass; }

	bool operator==(const Droplet& other) const { return m_Position == other.GetPosition(); }
private:
	glm::vec3 m_Position;
	static float m_Mass;
};

class Droplets
{
public:
	Droplets() { Init(); }
	Droplets(float radius) : m_Radius(radius) { Init(); }
	Droplets(std::vector<Droplet> droplets) : m_Droplets(droplets) { Init(); }
	Droplets(std::vector<Droplet> droplets, float radius) : m_Droplets(droplets), m_Radius(radius) { Init(); }
	Droplets(const int subdivisions, std::vector<Droplet> droplets)
		: m_Subdivisions(subdivisions), m_Droplets(droplets) { Init(); }
	Droplets(const int subdivisions, std::vector<Droplet> droplets, float radius)
		: m_Subdivisions(subdivisions), m_Droplets(droplets), m_Radius(radius) { Init(); }

	void AddDroplet(const Droplet& droplet) { m_Droplets.push_back(droplet); }
	void RemoveDroplet(const Droplet& droplet)
	{
		auto iter = std::find(m_Droplets.begin(), m_Droplets.end(), droplet);

		if (iter == m_Droplets.end()) return;
		m_Droplets.erase(iter);
	}

	void ClearDroplets() { m_Droplets.clear(); }

	std::shared_ptr<VAO> GetVAO() { return m_VAO; }

	void SetRadius(float radius) { m_Radius = radius; }

	void SetInstanceVBO(std::vector<Droplet> droplets);
	void UpdateInstanceVBO() const;

	void UpdateVertexVBO(float radius);

private:
	void Init()
	{
		SetSphereVBO();
		SetInstanceVBO(m_Droplets);
	}

	glm::vec3 SphereS(GLfloat u, GLfloat v) const;
	std::vector<glm::vec3> CreateSphere() const;

	void SetSphereVBO();

private:
	std::vector<Droplet> m_Droplets = std::vector<Droplet>();
	std::shared_ptr<VAO> m_VAO = std::make_shared<VAO>();
	VBO m_SphereVBO = VBO();
	VBO m_InstanceVBO = VBO();
	float m_Radius = 1.0f;
	int m_Subdivisions = 5;
};
