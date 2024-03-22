#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdint.h>
#include <vector>
#include <string>

#include "scene/vertex.h"

// Vertex Array Object
class VAO
{
public:
	VAO() { glGenVertexArrays(1, &m_ID); }
	~VAO() { glDeleteVertexArrays(1, &m_ID); }

	GLuint GetID() const { return m_ID; }

	void Bind() const { glBindVertexArray(m_ID); }

public:
	bool hasEBO = false;
	GLuint triangleCount = 0;
	GLuint vertexCount = 0;

private:
	GLuint m_ID = 0;
};

// Vertex Buffer Object
class VBO
{
public:
	VBO() { glGenBuffers(1, &m_ID); }
	~VBO() { glDeleteBuffers(1, &m_ID); }

	GLuint GetID() const { return m_ID; }

	void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_ID); }
	void SetBufferData(const std::vector<Vertex>& vertices) const
	{
		Bind();
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		SetVertexAttributesPointers();
	}

private:
	GLuint m_ID = 0;
};

// Element Buffer Object
class EBO
{
public:
	EBO() { glGenBuffers(1, &m_ID); }
	~EBO() { glDeleteBuffers(1, &m_ID); }

	GLuint GetID() const { return m_ID; }

	void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID); }
	void SetBufferData(const std::vector<GLuint>& indices) const
	{
		Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	}

private:
	GLuint m_ID = 0;
};

// Uniform Buffer Object
class UBO
{
public:
	UBO() { Init(); }
	UBO(const std::string& label) : m_Label(label) { Init(); }
	~UBO() { glDeleteBuffers(1, &m_ID); }

	GLuint GetID() const { return m_ID; }
	GLuint GetBindingPoint() const { return m_BindingPoint; }
	static GLuint GetBindingPointCount() { return s_BindingPointCount; }
	std::string GetLabel() const { return m_Label; }

	void Bind() const { glBindBuffer(GL_UNIFORM_BUFFER, m_ID); }
	void SetEmptyBuffer(GLsizeiptr size) const
	{
		Bind();
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
	}
	template<typename T>
	void SetBufferData(const T& data) const
	{
		Bind();
		glBufferData(GL_UNIFORM_BUFFER, sizeof(T), glm::value_ptr(data), GL_STATIC_DRAW);
	}
	template<typename T>
	void SetBufferSubData(GLintptr offset, const T& data) const
	{
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T), glm::value_ptr(data));
	}

	std::string GetLabel() { return m_Label; }
private:
	void Init()
	{
		glGenBuffers(1, &m_ID);
		m_BindingPoint = s_BindingPointCount++;
		glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint, m_ID);
	}

private:
	GLuint m_ID = 0;
	GLuint m_BindingPoint = 0;
	static GLuint s_BindingPointCount;
	std::string m_Label;
};

inline GLuint UBO::s_BindingPointCount = 0;