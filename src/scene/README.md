# Scene

Contains it's namesake the Scene class, as well as, anything the Scene class is conceptually related to. Part of the reason for this folders creation is the philosophy that the scene and renderer should not rely on, nor have any knowledge of, one another.

## Scene Class

The Scene class serves as a way to interface with all of the objects in the scene.

The Scene class has members
- `m_Terrain`: A Terrain instance which corresponds to the ground of the scene
- `m_VAOs`: An `std::shared_ptr` to an `std::vector` of VAO instances. These VAOs correspond to the objects represented in the scene (i.e. the water droplets and the terrain).

## Terrain Class

The Terrain class contains all information relating to rendering and simulating the terrain.

## Vertex Class

The Vertex class is used to make setting attribute pointers easier.

## Note

Water droplets positions should be tracked. When a water droplet gets close enough to a terrain vertex, the terrain vertex's water level is decreased and the water droplet is deleted.

Pools of water are then computed per frame/time step. A collection of triangle meshes, each with it's outer ring positions lying on the terrain.