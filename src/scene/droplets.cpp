#include "droplets.h"

void Droplets::SetInstanceVBO(const std::vector<uint64_t>& IDs)
{
	std::vector<glm::vec3> centers = GetCenters(IDs);

	// Set offset buffer
	m_VAO->isInstanced = true;
	m_VAO->instanceCount = static_cast<GLsizei>(centers.size());

	m_InstanceVBO.SetBufferData(centers);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);
}

void Droplets::UpdateInstanceVBO(const std::vector<uint64_t>& IDs) const
{
	std::vector<glm::vec3> centers = GetCenters(IDs);

	m_VAO->isInstanced = true;
	m_VAO->instanceCount = static_cast<GLsizei>(centers.size());

	m_InstanceVBO.SetBufferData(centers);
}

void Droplets::UpdateVertexVBO(float radius)
{
	m_Radius = radius;

	SetSphereVBO();
}

// u goes from [0.0, 1.0] and v goes from [0.0, 1.0]
glm::vec3 Droplets::SphereS(GLfloat u, GLfloat v) const
{
	GLfloat p = 2.0f * sqrtf(u * (1.0f - u));

	return glm::vec3(
		p * sinf(TAU * v),	// x: [-1.0, 1.0]
		1.0f - 2.0f * u,	// y: [-1.0, 1.0]
		p * cosf(TAU * v)	// z: [-1.0, 1.0]
	) * m_Radius;
}

std::vector<glm::vec3> Droplets::CreateSphere() const
{
	const int limit = m_Subdivisions + 1;

	std::vector<glm::vec3> vertices = std::vector<glm::vec3>();

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
			vertices.push_back(points[iNext][j    ]); // BL
			vertices.push_back(points[i    ][jNext]); // TR
			vertices.push_back(points[iNext][jNext]); // BR
			
			// Top Left Triangle
			vertices.push_back(points[iNext][j    ]); // BL
			vertices.push_back(points[i    ][j    ]); // TL
			vertices.push_back(points[i    ][jNext]); // TR
		}
	}

	return vertices;
}

void Droplets::SetSphereVBO()
{
	std::vector<glm::vec3> vertices = CreateSphere();

	m_VAO->Bind();

	// Set position buffer
	m_VAO->hasEBO = false;
	m_VAO->vertexCount = static_cast<GLuint>(vertices.size());
	m_VAO->triangleCount = m_VAO->vertexCount / 3U;

	m_SphereVBO.SetBufferData(vertices);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
	glEnableVertexAttribArray(0);
}
