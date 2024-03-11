#pragma once

#include <vector>

#include "..\glObjects.h"
#include "vertex.h"

class Terrain
{
public:
	Terrain() = default;
public:
	VAO m_VAO = VAO();
	std::vector<Vertex> m_Vertices = std::vector<Vertex>();
};
