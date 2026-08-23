# Puddle Simulator!
This is a shallow water simulator for CS 434.

Using the Smoothed Particle Hydrodynamics (SPH) method, this simulator is able to simulate the behavior of water droplets on a terrain. The terrain is generated using Perlin noise and can be textured with a variety of textures.

<img width="791" height="795" alt="image" src="https://github.com/user-attachments/assets/1c4a635a-5477-40ae-b5cc-de0a6e1627c4" />


## Project Structure
The primary control flow is the following:
> main() -> Application\:\:Run() -> Renderer\:\:Render()

## TODO
- [X] Terrain Generation
- [X] Textures
- [ ] Terrain Shader
- [ ] Physics Engine
	- [X] Simulating droplets
	- [ ] Simulating SPH on the ground
- [ ] Figuring out how SPH is going to be rendered 
