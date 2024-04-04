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
	DynamicSphere() : Sphere() { m_Mass = 0.0f; }
	DynamicSphere(glm::vec3 center) : Sphere(center) {}
	DynamicSphere(float radius) : Sphere(radius) {}
	DynamicSphere(glm::vec3 center, float radius) : Sphere(center, radius) {}

	glm::vec3 GetActiveForce() const { return m_ActiveForces; }

	void ApplyForce(glm::vec3 force) { m_ActiveForces += force; }
	void ClearForces() { m_ActiveForces = glm::vec3(); }

	glm::vec3 GetVelocity() const { return m_Velocity; }
	void SetVelocity(glm::vec3 velocity) { m_Velocity = velocity; }
	void UpdateVelocity(glm::vec3 prevAcceleration, const float deltaTime) { m_Velocity += 0.5f * (prevAcceleration + m_Acceleration) * deltaTime; }

	void SetAcceleration(glm::vec3 acceleration) { m_Acceleration = acceleration; }
	glm::vec3 GetAcceleration() const { return m_Acceleration; }
	void UpdateAcceleration() { m_Acceleration = m_ActiveForces / m_Density; }

	void UpdatePosition(const float deltaTime) { SetPosition(GetPosition() + m_Velocity * deltaTime + 0.5f * m_Acceleration * deltaTime * deltaTime); }

	void SetDensity(float density) { m_Density = density; }
	float GetDensity() const { return m_Density; }

	static void SetMass(float mass) { m_Mass = mass; }
	static float GetMass() { return m_Mass; }

	// Update velocity and position using leap frog scheme
	void Update(const float deltaTime)
	{
		glm::vec3 prevAcceleration = GetAcceleration();
		UpdateAcceleration();
		UpdateVelocity(prevAcceleration, deltaTime);
		UpdatePosition(deltaTime);
	}

	bool operator==(const DynamicSphere& other) const
	{
		return Compare(&other) &&
			m_ActiveForces == other.m_ActiveForces &&
			m_Acceleration == other.m_Acceleration &&
			m_Velocity == other.m_Velocity &&
			m_Density == other.m_Density;
	}

private:
	glm::vec3 m_ActiveForces = glm::vec3(0.0f);
	glm::vec3 m_Acceleration = glm::vec3(0.0f);
	glm::vec3 m_Velocity = glm::vec3(0.0f);
	float m_Density = 1.0f;
	inline static float m_Mass;
};