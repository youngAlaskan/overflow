#pragma once

#include <glm/glm.hpp>

class Sphere
{
public:
	Sphere() = default;
	Sphere(glm::vec3 center) : m_Center(center) {}
	Sphere(float radius) : m_Radius(radius) {}
	Sphere(glm::vec3 center, float radius) : m_Center(center), m_Radius(radius) {}

	void SetPosition(glm::vec3 center) { m_Center = center; }
	glm::vec3 GetPosition() const { return m_Center; }

	void SetRadius(float radius) { m_Radius = radius; }
	float GetRadius() const { return m_Radius; }

	bool operator==(const Sphere& other) const
	{
		return Compare(&other);
	}

	bool Compare(const Sphere* other) const
	{
		return m_Center == other->m_Center && m_Radius == other->m_Radius;
	}

private:
	glm::vec3 m_Center = glm::vec3();
	float m_Radius = 0.0f;
};

class DynamicSphere : public Sphere
{
public:
	DynamicSphere() = default;
	DynamicSphere(glm::vec3 center) : Sphere(center) {}
	DynamicSphere(float radius) : Sphere(radius) {}
	DynamicSphere(glm::vec3 center, float radius) : Sphere(center, radius) {}

	glm::vec3 GetActiveForce() const { return m_ActiveForces; }
	void ApplyForce(glm::vec3 force) { m_ActiveForces += force; }
	void ClearForces() { m_ActiveForces = glm::vec3(0.0f); }

	glm::vec3 GetVelocity() const { return m_Velocity; }
	void SetVelocity(glm::vec3 velocity) { m_Velocity = velocity; }
	glm::vec3 GetAcceleration() const { return m_Acceleration; }

	void SetDensity(float density) { m_Density = density; }
	float GetDensity() const { return m_Density; }

	void SetMass(float mass) { m_Mass = mass; }
	float GetMass() const { return m_Mass; }

	// Semi-implicit Euler
	void Update(const float deltaTime)
	{
		m_Acceleration = m_ActiveForces / m_Mass;
		m_Velocity += m_Acceleration * deltaTime;
		SetPosition(GetPosition() + m_Velocity * deltaTime);
	}

	bool operator==(const DynamicSphere& other) const
	{
		return Compare(&other) &&
			m_ActiveForces == other.m_ActiveForces &&
			m_Acceleration == other.m_Acceleration &&
			m_Velocity == other.m_Velocity &&
			m_Density == other.m_Density &&
			m_Mass == other.m_Mass;
	}

private:
	glm::vec3 m_ActiveForces = glm::vec3(0.0f);
	glm::vec3 m_Acceleration = glm::vec3(0.0f);
	glm::vec3 m_Velocity = glm::vec3(0.0f);
	float m_Density = 1.0f;
	float m_Mass = 1.0f;
};