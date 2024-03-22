#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "../glObjects.h"

constexpr float PI = 3.14159274101257324219f;

class Droplets
{
public:
	Droplets(std::vector<glm::vec3> centers) { SetVAO(centers); }

public:
	std::vector<glm::vec3> m_Vertices = std::vector<glm::vec3>();
	std::vector<glm::vec3> m_Centers = std::vector<glm::vec3>();
	std::shared_ptr<VAO> m_VAO = std::make_shared<VAO>();

private:
	// u goes from [0.0, 1.0]
	// returns P in range [0.0, 1.0]
	static GLfloat SphereP(GLfloat u)
	{
		u = 1.0f - 2.0f * u; // reparametrize to <‐1.0, 1.0>
		return (sqrt(1.0f - u * u));
	}

	// u goes from [0.0, 1.0] and v goes from [0.0, 1.0]
	static glm::vec3 SphereS(GLfloat u, GLfloat v)
	{
		return glm::vec3(SphereP(u) * sin(2 * PI * v),	// x: [-1.0, 1.0]
			1.0f - 2.0f * u,						    // y: [-1.0, 1.0]
			SphereP(u) * cos(2 * PI * v));				// z: [-1.0, 1.0]
	}

	void CreateSphere()
	{
		float g_verticalSubdivisions = static_cast<float>(5);
		float g_horizontalSubdivisions = static_cast<float>(5);
		for (int i = 0; i < 5; i++)
		{
			GLfloat u = static_cast<float>(i) / g_verticalSubdivisions;
			GLfloat uNext = static_cast<float>(i + 1) / g_verticalSubdivisions;
			for (int j = 0; j < 5; j++)
			{
				GLfloat v = static_cast<float>(j) / g_horizontalSubdivisions;
				GLfloat vNext = static_cast<float>(j + 1) / g_horizontalSubdivisions;
				glm::vec3 vertexBL = SphereS(u, v);         // TL---TR
				glm::vec3 vertexBR = SphereS(u, vNext);     // |   / |
				glm::vec3 vertexTR = SphereS(uNext, vNext); // | /   |
				glm::vec3 vertexTL = SphereS(uNext, v);     // BL---BR
				m_Vertices.push_back(vertexBL);
				m_Vertices.push_back(vertexBR);
				m_Vertices.push_back(vertexTR);
				m_Vertices.push_back(vertexBL);
				m_Vertices.push_back(vertexTR);
				m_Vertices.push_back(vertexTL);
			}
		}
	}

	void SetVAO(std::vector<glm::vec3> centers)
	{
		m_Centers = centers;

		VBO vertexVBO = VBO();
		VBO instanceVBO = VBO();

		CreateSphere();

		m_VAO->Bind();

		m_VAO->hasEBO = false;
		m_VAO->vertexCount = static_cast<GLuint>(m_Vertices.size());
		m_VAO->triangleCount = m_VAO->vertexCount / 3U;
		m_VAO->isInstanced = true;
		m_VAO->instanceCount = static_cast<GLsizei>(m_Centers.size());

		vertexVBO.SetBufferData(m_Vertices);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
		glEnableVertexAttribArray(0);

		instanceVBO.SetBufferData(m_Centers);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
		glEnableVertexAttribArray(1);
		glVertexAttribDivisor(1, 1);
	}
};
