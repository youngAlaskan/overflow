#pragma once

#include <vector>

#include "..\glObjects.h"
#include "vertex.h"

class Terrain
{
public:
	Terrain() : m_Vertices() {}
	Terrain(const std::vector<Vertex>& vertices) { SetVertices(vertices); }

	void SetVertices(std::vector<Vertex> vertices)
	{
		m_Vertices = vertices;

		m_VAO.Bind();

		m_VBO.SetBufferData(m_Vertices);
	}
public:
	VAO m_VAO = VAO();
	VBO m_VBO = VBO();
	std::vector<Vertex> m_Vertices;
};
