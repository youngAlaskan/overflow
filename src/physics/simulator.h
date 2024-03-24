#pragma once

#include <physx/PxPhysicsAPI.h>

class Simulator
{
public:
	Simulator() { Init(); }
	~Simulator() { CleanUp(); }

	void Step();

	static physx::PxRigidDynamic* CreateDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxMaterial& material, const physx::PxVec3& velocity = physx::PxVec3(0));
	static physx::PxRigidStatic* CreateStatic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxMaterial& material);
public:

private:
	void Init();

	void CleanUp();
private:
	static physx::PxFoundation*				m_Foundation;
	static physx::PxPhysics*				m_Physics;
	static physx::PxDefaultCpuDispatcher*	m_Dispatcher;
	static physx::PxScene*					m_Scene;
};
