#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <stdint.h>
#include <vector>

// Vertex Array Object
class VAO
{
public:
	VAO() = default;
	~VAO() { glDeleteVertexArrays(1, &m_ID); }

	// Generate Vertex Array Object. Not in constructor to prevent
	// glad function from being called before glad is lauched.
	void Init() { glGenVertexArrays(1, &m_ID); }

	void Bind() const { glBindVertexArray(m_ID); }

public:
	GLuint m_ID = 0;
	bool hasEBO = false;
	GLuint triangleCount = 0;
};

// Vertex Buffer Object
class VBO
{
public:
	VBO() = default;
	~VBO() { glDeleteBuffers(1, &m_ID); }

	// Generate Vertex Buffer Object. Not in constructor to prevent
	// glad function from being called before glad is lauched.
	void Init() { glGenBuffers(1, &m_ID); }

	void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_ID); }
	void SetBufferData(const std::vector<glm::vec3>& vertices) const
	{
		Bind();
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
	}

public:
	GLuint m_ID = 0;
};

// Element Buffer Object
class EBO
{
public:
	EBO() = default;
	~EBO() { glDeleteBuffers(1, &m_ID); }

	// Generate Element Buffer Object. Not in constructor to prevent
	// glad function from being called before glad is lauched.
	void Init() { glGenBuffers(1, &m_ID); }

	void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID); }
	void SetBufferData(const std::vector<uint32_t>& indices) const
	{
		Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
	}

public:
	GLuint m_ID = 0;
};