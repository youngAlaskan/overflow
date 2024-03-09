#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <stdint.h>
#include <vector>

// OpenGL Object base class
class GLO
{
public:
	GLuint GetID() { return m_ID; }

	virtual void Bind() const = 0;
protected:
	GLuint m_ID = 0;
};

// Vertex Array Object
class VAO : protected GLO
{
public:
	VAO() { glGenVertexArrays(1, &m_ID); }
	~VAO() { glDeleteVertexArrays(1, &m_ID); }

	void Bind() const { glBindVertexArray(m_ID); }

public:
	bool hasEBO = false;
	GLuint triangleCount = 0;
};

// Vertex Buffer Object
class VBO : protected GLO
{
public:
	VBO() { glGenBuffers(1, &m_ID); }
	~VBO() { glDeleteBuffers(1, &m_ID); }

	void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_ID); }
	void SetBufferData(const std::vector<glm::vec3>& vertices) const
	{
		Bind();
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
	}
};

// Element Buffer Object
class EBO : protected GLO
{
public:
	EBO() { glGenBuffers(1, &m_ID); }
	~EBO() { glDeleteBuffers(1, &m_ID); }

	void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID); }
	void SetBufferData(const std::vector<uint32_t>& indices) const
	{
		Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
	}
};