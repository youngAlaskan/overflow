#include "droplets.h"

void Droplets::SetInstanceVBO(std::vector<glm::vec3> centers)
{
	m_Centers = centers;

	// Set offset buffer
	m_VAO->isInstanced = true;
	m_VAO->instanceCount = static_cast<GLsizei>(m_Centers.size());

	m_InstanceVBO.SetBufferData(m_Centers);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);
}

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
	const int limit = m_Subdivisions + 1;

	// Creates a sphere with 5 subdivisions on each axis.
	auto points = std::vector<std::vector<glm::vec3>>(limit, std::vector<glm::vec3>(limit));

	auto u = std::vector<GLfloat>(limit);
	auto v = std::vector<GLfloat>(limit);
	for (int i = 0; i < limit; i++)
	{
		u[i] = static_cast<float>(i) / m_Subdivisions;
		v[i] = static_cast<float>(i) / m_Subdivisions;
	}

	// Create every point
	for (int i = 0; i < limit; i++)
	{
		for (int j = 0; j < limit; j++)
		{
			points[i][j] = SphereS(u[i], v[j]);
		}
	}

	// Add every triangle to vertices
	for (int i = 0, iNext = 1; i < m_Subdivisions; i++, iNext++)
	{
		for (int j = 0, jNext = 1; j < m_Subdivisions; j++, jNext++)
		{
			// Bottom Right Triangle
			m_Vertices.push_back(points[iNext][j    ]); // BL
			m_Vertices.push_back(points[i    ][jNext]); // TR
			m_Vertices.push_back(points[iNext][jNext]); // BR
			
			// Top Left Triangle
			m_Vertices.push_back(points[iNext][j    ]); // BL
			m_Vertices.push_back(points[i    ][j    ]); // TL
			m_Vertices.push_back(points[i    ][jNext]); // TR
		}
	}
}

void Droplets::SetSphereVBO()
{
	CreateSphere();

	m_VAO->Bind();

	// Set position buffer
	m_VAO->hasEBO = false;
	m_VAO->vertexCount = static_cast<GLuint>(m_Vertices.size());
	m_VAO->triangleCount = m_VAO->vertexCount / 3U;

	m_SphereVBO.SetBufferData(m_Vertices);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
	glEnableVertexAttribArray(0);
}
