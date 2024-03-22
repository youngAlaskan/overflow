#include "droplets.h"

// u goes from [0.0, 1.0] and v goes from [0.0, 1.0]
inline glm::vec3 SphereS(GLfloat u, GLfloat v)
{
	GLfloat p = 2.0f * sqrtf(u * (1.0f - u));

	return glm::vec3(
		p * sinf(TAU * v),	// x: [-1.0, 1.0]
		1.0f - 2.0f * u,	// y: [-1.0, 1.0]
		p * cosf(TAU * v)	// z: [-1.0, 1.0]
	);
}

void Droplets::CreateSphere()
{
	// Creates a sphere with 5 subdivisions on each axis.

	glm::vec3 points[6][6] = {};

	GLfloat u[6] = { 0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f };
	GLfloat v[6] = { 0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f };

	// Create every point
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			points[i][j] = SphereS(u[i], v[j]);
		}
	}

	// Add every triangle to vertices
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			// Bottom Right Triangle
			m_Vertices.push_back(points[i + 1][j    ]); // BL
			m_Vertices.push_back(points[i    ][j + 1]); // TR
			m_Vertices.push_back(points[i + 1][j + 1]); // BR
			
			// Top Left Triangle
			m_Vertices.push_back(points[i + 1][j    ]); // BL
			m_Vertices.push_back(points[i    ][j    ]); // TL
			m_Vertices.push_back(points[i    ][j + 1]); // TR
		}
	}
}

void Droplets::SetVAO(std::vector<glm::vec3> centers)
{
	m_Centers = centers;

	CreateSphere();

	m_VAO->Bind();

	// Set position buffer
	VBO positionVBO = VBO();

	m_VAO->hasEBO = false;
	m_VAO->vertexCount = static_cast<GLuint>(m_Vertices.size());
	m_VAO->triangleCount = m_VAO->vertexCount / 3U;

	positionVBO.SetBufferData(m_Vertices);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
	glEnableVertexAttribArray(0);

	// Set offset buffer
	VBO instanceVBO = VBO();
	
	m_VAO->isInstanced = true;
	m_VAO->instanceCount = static_cast<GLsizei>(m_Centers.size());
	
	instanceVBO.SetBufferData(m_Centers);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);
}
