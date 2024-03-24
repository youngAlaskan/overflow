#include "simulator.h"

#include <assert.h>

void Simulator::Step()
{
	m_Scene->simulate(1.0f / 60.0f);
	m_Scene->fetchResults(true);
}

physx::PxRigidDynamic* Simulator::CreateDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxMaterial& material, const physx::PxVec3& velocity)
{
	physx::PxRigidDynamic* rigidDynamic = m_Physics->createRigidDynamic(t);
	physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*rigidDynamic, geometry, material);
	physx::PxRigidBodyExt::updateMassAndInertia(*rigidDynamic, 10.0f);
	rigidDynamic->setLinearVelocity(velocity);
	m_Scene->addActor(*rigidDynamic);
	return rigidDynamic;
}

physx::PxRigidStatic* Simulator::CreateStatic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxMaterial& material)
{
	physx::PxRigidStatic* rigidStatic = m_Physics->createRigidStatic(t);
	physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*rigidStatic, geometry, material);
	m_Scene->addActor(*rigidStatic);
	return rigidStatic;

}

void Simulator::Init()
{
	m_Foundation = NULL;
	m_Physics    = NULL;
	m_Dispatcher = NULL;
	m_Scene      = NULL;

	physx::PxReal stackZ = 10.0f;

	static physx::PxDefaultAllocator allocator;
	static physx::PxDefaultErrorCallback errorCallback;

	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback);
	if (!m_Foundation)
		throw ("PxCreateFoundation failed!");

	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, physx::PxTolerancesScale(), true);
	if (!m_Physics)
		throw ("PxCreatePhysics failed!");

	// PxRegisterHeightFields(*m_Physics);

	physx::PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	m_Dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_Dispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	m_Scene = m_Physics->createScene(sceneDesc);
}

void Simulator::CleanUp()
{
	PX_RELEASE(m_Scene);
	PX_RELEASE(m_Dispatcher);
	PX_RELEASE(m_Physics);
	PX_RELEASE(m_Foundation); // Must be last
}