#include "simulator.h"

inline static physx::PxFoundation* g_Foundation           = NULL;
inline static physx::PxPhysics* g_Physics                 = NULL;
inline static physx::PxDefaultCpuDispatcher* g_Dispatcher = NULL;
inline static physx::PxScene* g_Scene                     = NULL;

void Simulator::Step()
{
	g_Scene->simulate(1.0f / 1024.0f);
	g_Scene->fetchResults(true);
}

void Simulator::AddDroplet(physx::PxVec3 center, physx::PxReal radius)
{
	physx::PxMaterial* dropletMaterial = CreateMaterial(0.01f, 0.01f, 0.2f);
	physx::PxSphereGeometry sphereGeometry = physx::PxSphereGeometry(radius);

	m_Droplets.push_back(CreateDynamic(physx::PxTransform(center), sphereGeometry, *dropletMaterial));
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

physx::PxMaterial* Simulator::CreateMaterial(physx::PxReal staticFriction, physx::PxReal dynamicFriction, physx::PxReal restitution)
{
	return g_Physics->createMaterial(staticFriction, dynamicFriction, restitution);
}

physx::PxTriangleMesh* Simulator::CreateTriangleMesh(const std::vector<physx::PxVec3>& vertices, const std::vector<physx::PxU32>& indices)
{
	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<physx::PxU32>(vertices.size());
	meshDesc.points.stride = sizeof(physx::PxVec3);
	meshDesc.points.data = vertices.data();

	meshDesc.triangles.count = static_cast<physx::PxU32>(indices.size()) / 3U;
	meshDesc.triangles.stride = 3U * sizeof(physx::PxU32);
	meshDesc.triangles.data = indices.data();

	physx::PxTolerancesScale scale;
	physx::PxCookingParams params(scale);

	physx::PxDefaultMemoryOutputStream writeBuffer;
	physx::PxTriangleMeshCookingResult::Enum result;
	bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
	if (!status)
		return NULL;

	physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	return g_Physics->createTriangleMesh(readBuffer);
}

void Simulator::ClearDroplets()
{
	for (auto droplet : m_Droplets)
	{
		g_Scene->removeActor(*droplet);
	}
	m_Droplets.clear();
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