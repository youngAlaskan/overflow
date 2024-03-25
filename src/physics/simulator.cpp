#include "simulator.h"

#include <assert.h>

inline static physx::PxFoundation* g_Foundation           = NULL;
inline static physx::PxPhysics* g_Physics                 = NULL;
inline static physx::PxDefaultCpuDispatcher* g_Dispatcher = NULL;
inline static physx::PxScene* g_Scene                     = NULL;

void Simulator::Step()
{
	g_Scene->simulate(1.0f / 60.0f);
	g_Scene->fetchResults(true);
}

physx::PxRigidDynamic* Simulator::CreateDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxMaterial& material, const physx::PxVec3& velocity)
{
	physx::PxRigidDynamic* rigidDynamic = g_Physics->createRigidDynamic(t);
	physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*rigidDynamic, geometry, material);
	physx::PxRigidBodyExt::updateMassAndInertia(*rigidDynamic, 10.0f);
	rigidDynamic->setLinearVelocity(velocity);
	g_Scene->addActor(*rigidDynamic);
	return rigidDynamic;
}

physx::PxRigidStatic* Simulator::CreateStatic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxMaterial& material)
{
	physx::PxRigidStatic* rigidStatic = g_Physics->createRigidStatic(t);
	physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*rigidStatic, geometry, material);
	g_Scene->addActor(*rigidStatic);
	return rigidStatic;

}

void Simulator::Init()
{
	static physx::PxDefaultAllocator allocator;
	static physx::PxDefaultErrorCallback errorCallback;

	g_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback);
	if (!g_Foundation)
		throw ("PxCreateFoundation failed!");

	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_Foundation, physx::PxTolerancesScale(), true);
	if (!g_Physics)
		throw ("PxCreatePhysics failed!");

	physx::PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	g_Dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = g_Dispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	g_Scene = g_Physics->createScene(sceneDesc);
}

void Simulator::CleanUp()
{
	PX_RELEASE(g_Scene);
	PX_RELEASE(g_Dispatcher);
	PX_RELEASE(g_Physics);
	PX_RELEASE(g_Foundation); // Must be last
}