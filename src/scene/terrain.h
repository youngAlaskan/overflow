#pragma once

#include <vector>
#include <memory>

#include "..\glObjects.h"
#include "vertex.h"

class Terrain
{
public:
	Terrain() : m_Vertices() {}
	Terrain(const std::vector<Vertex>& vertices) { SetVertices(vertices); }
	Terrain(const GLuint length, const GLfloat minHeight, const GLfloat maxHeight, const GLfloat scale = 1.0f) { SetRandomVertices(length, minHeight, maxHeight, scale); }

	void SetVertices(const std::vector<Vertex>& vertices);

	void SetRandomVertices(const GLuint length, const GLfloat minHeight, const GLfloat maxHeight, const GLfloat scale = 1.0f);

public:
	std::shared_ptr<VAO> m_VAO = std::make_shared<VAO>();
	VBO m_VBO = VBO();
	std::vector<Vertex> m_Vertices;
};
