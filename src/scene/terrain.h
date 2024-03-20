#pragma once

#include <vector>

#include "..\glObjects.h"
#include "vertex.h"

class Terrain
{
public:
	Terrain() : m_Vertices() {}
	Terrain(const std::vector<Vertex>& vertices) { SetVertices(vertices); }

	void SetVertices(const std::vector<Vertex>& vertices)
	{
		m_Vertices = vertices;

		m_VAO->Bind();

		m_VAO->hasEBO = false;
		m_VAO->vertexCount = m_Vertices.size();
		m_VAO->triangleCount = m_VAO->vertexCount / 3;

		m_VBO.SetBufferData(m_Vertices);
	}
public:
	std::shared_ptr<VAO> m_VAO = std::make_shared<VAO>();
	VBO m_VBO = VBO();
	std::vector<Vertex> m_Vertices;
};
