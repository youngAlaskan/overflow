#pragma once

#include <glm/glm.hpp>

class Sphere
{
public:
	Sphere() = default;
	Sphere(glm::vec3 center) : m_Center(center) {}
	Sphere(float radius) : m_Radius(radius) {}
	Sphere(glm::vec3 center, float radius) : m_Center(center), m_Radius(radius) {}

	void SetCenter(glm::vec3 center) { m_Center = center; }
	glm::vec3 GetCenter() const { return m_Center; }

	void SetRadius(float radius) { m_Radius = radius; }
	float GetRadius() const { return m_Radius; }

private:
	glm::vec3 m_Center = glm::vec3();
	float m_Radius = 0.0f;
};

class DynamicSphere : public Sphere
{
public:
	void ApplyForce(glm::vec3 force) { m_ActiveForces += force; }
	void ClearForces() { m_ActiveForces = glm::vec3(); }

private:
	glm::vec3 m_ActiveForces = glm::vec3();
};