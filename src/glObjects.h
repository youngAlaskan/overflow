#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <stdint.h>
#include <vector>

#include "scene/vertex.h"

// Vertex Array Object
class VAO
{
public:
	VAO() { glGenVertexArrays(1, &m_ID); }
	~VAO() { glDeleteVertexArrays(1, &m_ID); }

	void Bind() const { glBindVertexArray(m_ID); }

public:
	GLuint m_ID = 0;
	bool hasEBO = false;
	GLuint triangleCount = 0;
	GLuint vertexCount = 0;
};

// Vertex Buffer Object
class VBO
{
public:
	VBO() { glGenBuffers(1, &m_ID); }
	~VBO() { glDeleteBuffers(1, &m_ID); }

	void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_ID); }
	void SetBufferData(const std::vector<Vertex>& vertices) const
	{
		Bind();
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

		SetVertexAttributesPointers();
	}

public:
	GLuint m_ID = 0;
};

// Element Buffer Object
class EBO
{
public:
	EBO() { glGenBuffers(1, &m_ID); }
	~EBO() { glDeleteBuffers(1, &m_ID); }

	void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID); }
	void SetBufferData(const std::vector<GLuint>& indices) const
	{
		Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
	}

public:
	GLuint m_ID = 0;
};