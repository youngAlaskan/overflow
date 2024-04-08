#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>
#include <memory>

#include "../glObjects.h"

constexpr float TAU = 6.283185307179586f; // 2 * pi

class Droplets
{
public:
	Droplets() { Init(); }
	Droplets(const float radius) : m_Radius(radius) { Init(); }
	Droplets(const int subdivisions) : m_Subdivisions(subdivisions) { Init(); }
	Droplets(const int subdivisions, const float radius) : m_Subdivisions(subdivisions), m_Radius(radius) { Init(); }

	std::shared_ptr<std::unordered_map<uint64_t, glm::vec3>> GetIDsToCenters() { return m_IDstoCenters; }
	std::vector<glm::vec3> GetCenters(const std::vector<uint64_t>& IDs) const
	{
		std::vector<glm::vec3> centers = std::vector<glm::vec3>(IDs.size());

		for (int i = 0; i < IDs.size(); i++)
		{
			centers[i] = m_IDstoCenters->at(IDs[i]);
		}

		return centers;
	}

	void AddDroplets(const std::vector<std::pair<uint64_t, glm::vec3>>& IDsAndCenters)
	{
		for (const auto& IDAndCenter : IDsAndCenters)
		{
			AddDroplet(IDAndCenter);
		}
	}
	void AddDroplet(const std::pair<uint64_t, glm::vec3>& IDAndCenter)
	{
		m_IDstoCenters->insert(IDAndCenter);
	}
	void RemoveDroplet(const uint64_t& id)
	{
		m_IDstoCenters->erase(id);
	}

	void ClearDroplets() { m_IDstoCenters->clear(); }

	std::shared_ptr<VAO> GetVAO() { return m_VAO; }

	void SetRadius(float radius) { m_Radius = radius; }

	void SetInstanceVBO(const std::vector<uint64_t>& IDs);
	void UpdateInstanceVBO(const std::vector<uint64_t>& IDs) const;

	void UpdateVertexVBO(float radius);

private:
	void Init()
	{
		SetSphereVBO();
		SetInstanceVBO({});
	}

	glm::vec3 SphereS(GLfloat u, GLfloat v) const;
	std::vector<glm::vec3> CreateSphere() const;

	void SetSphereVBO();

private:
	std::shared_ptr<std::unordered_map<uint64_t, glm::vec3>> m_IDstoCenters = std::make_shared<std::unordered_map<uint64_t, glm::vec3>>();
	std::shared_ptr<VAO> m_VAO = std::make_shared<VAO>();
	VBO m_SphereVBO = VBO();
	VBO m_InstanceVBO = VBO();
	float m_Radius = 1.0f;
	int m_Subdivisions = 8;
};
