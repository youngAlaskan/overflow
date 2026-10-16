#pragma once

#include <glad\glad.h>
#include <memory>
#include <unordered_map>
#include "../glObjects.h"
#include "../rendering/marchingCubes.h"

class WaterSurface
{
public:
	WaterSurface()   // needs a GL context: construct it in Application::Init after glad loads
	{
		m_VAO->Bind();
		m_VBO.Bind();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SurfaceVertex), (void*)offsetof(SurfaceVertex, Position));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SurfaceVertex), (void*)offsetof(SurfaceVertex, Normal));
		glEnableVertexAttribArray(1);

		m_EBO.Bind();                 // the element buffer binding is stored in the VAO
		m_VAO->hasEBO = true;
		glBindVertexArray(0);
	}

	std::shared_ptr<VAO> GetVAO() const { return m_VAO; }

	void Update(const std::unordered_map<uint64_t, glm::vec3>& centers,
		const MarchingCubesMesher::Params& prm)
	{
		m_Points.clear();
		m_Points.reserve(centers.size());
		for (const auto& kv : centers) m_Points.push_back(kv.second);

		m_Mesher.Build(m_Points, prm, m_Mesh);

		m_VBO.SetBufferData(m_Mesh.Vertices, GL_STREAM_DRAW);
		
		m_VAO->Bind();                // bind first, or the EBO would attach to whatever VAO was last bound
		m_EBO.SetBufferData(m_Mesh.idx);
		glBindVertexArray(0);
		
		m_VAO->vertexCount = static_cast<GLuint>(m_Mesh.Vertices.size());
		m_VAO->triangleCount = static_cast<GLuint>(m_Mesh.idx.size() / 3);
	}

private:
	std::shared_ptr<VAO> m_VAO = std::make_shared<VAO>();
	VBO m_VBO;
	EBO m_EBO;
	MarchingCubesMesher m_Mesher;
	std::vector<glm::vec3> m_Points;
	SurfaceMesh m_Mesh;
};