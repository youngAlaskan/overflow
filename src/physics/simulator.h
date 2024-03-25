#pragma once

#include <physx/PxPhysicsAPI.h>

#include <vector>

class Simulator
{
public:
	Simulator() { Init(); }
	~Simulator() { CleanUp(); }

	void Step();

	void AddDroplet(physx::PxVec3 center);

	static physx::PxRigidDynamic* CreateDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxMaterial& material, const physx::PxVec3& velocity = physx::PxVec3(0));
	static physx::PxRigidStatic* CreateStatic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxMaterial& material);

	static physx::PxMaterial* CreateMaterial(physx::PxReal staticFriction, physx::PxReal dynamicFriction, physx::PxReal restitution);
	static physx::PxTriangleMesh* CreateTriangleMesh(const std::vector<physx::PxVec3>& vertices, const std::vector<physx::PxU32>& indices);
public:
	std::vector<physx::PxRigidDynamic*> m_Droplets = std::vector<physx::PxRigidDynamic*>();

private:
	void Init();

	void CleanUp();
};
